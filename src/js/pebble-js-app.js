var mConfig = {};
var locationOptions = { "timeout": 15000, "maximumAge": 60000 }; 
var locationWatcher;

function fetchWeather(latitude, longitude) {

  var response;
  var req = new XMLHttpRequest();
  req.open('GET', "http://www.mirz.com/Chunk2/Yahoo.php?" +
    "lat=" + latitude + "&long=" + longitude + "&units=" + UnitsToString(mConfig.units), true);
  req.onload = function(e) {
    if (req.readyState == 4) {
      if(req.status == 200) {
        console.log(req.responseText);
        response = JSON.parse(req.responseText);
        var temperature, icon, high, low;
        if (response) {
          var weatherResult = response;
          temperature = weatherResult.temp;
          code = weatherResult.code;
          high = weatherResult.high;
          low = weatherResult.low;

          Pebble.sendAppMessage({
            "temperature": temperature,
            "icon": code,
            "high": high,
            "low": low
            });
        }

      } else {
        console.log("Weather Error");
      }
    }
  }
  req.send(null);
}

function locationSuccess(pos) {
  //console.log("JS locationSuccess()");
  var coordinates = pos.coords;
  fetchWeather(coordinates.latitude, coordinates.longitude);
}

function locationError(err) {
  //console.warn('JS locationError(' + err.code + '): ' + err.message);
  Pebble.sendAppMessage({
    "temperature": 999,
    "icon": 48,
    "high": 0,
    "low": 0
    });
}

function saveLocalData(config) {
  localStorage.setItem("style", parseInt(config.style));  
  localStorage.setItem("bluetoothvibe", parseInt(config.bluetoothvibe)); 
  localStorage.setItem("hourlyvibe", parseInt(config.hourlyvibe)); 
  localStorage.setItem("units", parseInt(config.units));  
  localStorage.setItem("blink", parseInt(config.blink));
  localStorage.setItem("dateformat", parseInt(config.dateformat));
  
  loadLocalData();
}
function loadLocalData() {
  mConfig.style = parseInt(localStorage.getItem("style"));
  mConfig.bluetoothvibe = parseInt(localStorage.getItem("bluetoothvibe"));
  mConfig.hourlyvibe = parseInt(localStorage.getItem("hourlyvibe"));
  mConfig.units = parseInt(localStorage.getItem("units"));
  mConfig.blink = parseInt(localStorage.getItem("blink"));
  mConfig.dateformat = parseInt(localStorage.getItem("dateformat"));
  mConfig.configureUrl = "http://www.mirz.com/Chunk2/index.html";
}
function returnConfigToPebble() {
  Pebble.sendAppMessage({
    "style":parseInt(mConfig.style), 
    "bluetoothvibe":parseInt(mConfig.bluetoothvibe), 
    "hourlyvibe":parseInt(mConfig.hourlyvibe),
    "units":parseInt(mConfig.units),
    "blink":parseInt(mConfig.blink),
    "dateformat":parseInt(mConfig.dateformat)
  });
}
function UnitsToString(unit) {
  if(unit==0) {
    return "f";
  }
  return "c";
}


Pebble.addEventListener("ready", function(e) {
  loadLocalData();
  returnConfigToPebble();
});


Pebble.addEventListener("appmessage", function(e) {
  window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
});


Pebble.addEventListener("showConfiguration", function(e) {
	Pebble.openURL(mConfig.configureUrl);
});

Pebble.addEventListener("webviewclosed", function(e) {
  if (e.response) {
    var config = JSON.parse(e.response);
    saveLocalData(config);
    returnConfigToPebble();
  }
});


