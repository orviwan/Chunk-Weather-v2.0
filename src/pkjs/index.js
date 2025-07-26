var Clay = require('pebble-clay');
var clayConfig = require('./config.js');
var clay = new Clay(clayConfig);

Pebble.addEventListener('ready', function(e) {
  console.log('JS Ready');
  Pebble.sendAppMessage({'JSReady': 1});
});

Pebble.addEventListener('appmessage', function(e) {
  var locationOptions = {'timeout': 15000, 'maximumAge': 60000};
  navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
});

// ********************************************************************************* //
// ** This is the main function that has been updated for the Open-Meteo API ** //
// ********************************************************************************* //
function fetchWeather(latitude, longitude) {
  var settings = {};
  var tempUnits = 'celsius'; // Default to Celsius

  try {
    settings = JSON.parse(localStorage.getItem('clay-settings')) || {};
    // Get the unit setting from Clay config
    tempUnits = unitsToString(settings.WeatherIsFahrenheit);
  } catch (e) {
    // Stick with the default if settings fail to parse
  }

  // Construct the new URL for the Open-Meteo API
  var url = 'https://api.open-meteo.com/v1/forecast' +
            '?latitude=' + latitude +
            '&longitude=' + longitude +
            '&current=temperature_2m,weather_code' +
            '&daily=temperature_2m_max,temperature_2m_min' +
            '&temperature_unit=' + tempUnits +
            '&forecast_days=1' + // We only need today's high/low
            '&timezone=auto';

  xhrRequest(url, 'GET',
    function(responseText) {
      var json = JSON.parse(responseText);

      var temp = Math.round(json.current.temperature_2m);
      var high = Math.round(json.daily.temperature_2m_max[0]);
      var low = Math.round(json.daily.temperature_2m_min[0]);
      
      // The new API uses WMO weather codes, which must be mapped
      // to the codes your watchface expects (likely the old Yahoo codes).
      var icon = mapWeatherIcon(json.current.weather_code);

      var data = {
        'WeatherTemperature': temp,
        'WeatherIcon': icon,
        'WeatherHigh': high,
        'WeatherLow': low
      };

      console.log('Sending weather data to Pebble: ' + JSON.stringify(data));
      Pebble.sendAppMessage(data);
    }
  );
}

function locationSuccess(pos) {
  fetchWeather(pos.coords.latitude, pos.coords.longitude);
}

function locationError(err) {
  console.error('Location error: ' + err.message);
  Pebble.sendAppMessage({
    'WeatherTemperature': 999,
    'WeatherIcon': 48, // 48 is often 'not available'
    'WeatherHigh': 0,
    'WeatherLow': 0
  });
}

/**
 * Open-Meteo uses different weather codes (WMO codes) than Yahoo Weather.
 * You MUST map these to the icon codes your watchface uses.
 * This is a sample mapping, you may need to adjust it.
 * WMO Codes: https://open-meteo.com/en/docs#weathervariables
 */
function mapWeatherIcon(weatherCode) {
  // This is a sample mapping. You will need to complete it based on your watchface's icons.
  // The numbers on the right (e.g., 32, 30, 28) are example Yahoo Weather codes.
  switch(weatherCode) {
    case 0: return 32; // Clear sky -> Sunny
    case 1: return 30; // Mainly clear -> Partly Cloudy
    case 2: return 28; // Partly cloudy -> Mostly Cloudy
    case 3: return 26; // Overcast -> Cloudy
    case 45: case 48: return 20; // Fog -> Foggy
    case 51: case 53: case 55: return 9; // Drizzle -> Drizzle/Showers
    case 61: case 63: case 65: return 11; // Rain -> Showers
    case 66: case 67: return 12; // Freezing Rain -> Showers
    case 71: case 73: case 75: return 14; // Snow fall -> Light Snow Showers
    case 80: case 81: case 82: return 40; // Rain showers -> Scattered Showers
    case 85: case 86: return 41; // Snow showers -> Heavy Snow
    case 95: case 96: case 99: return 4; // Thunderstorm -> Thunderstorms
    default: return 3200; // Default/Unknown icon
  }
}


// ** Updated to return strings the new API expects **
function unitsToString(unit) {
  if (unit) {
    return 'fahrenheit';
  }
  return 'celsius';
}

// This helper function remains the same
var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};
