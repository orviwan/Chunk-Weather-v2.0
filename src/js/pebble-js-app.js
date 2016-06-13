var mConfig = {};
var locationOptions = { "timeout": 15000, "maximumAge": 60000 };

/* Convenient function to automatically retry messages. */
Pebble.sendAppMessageWithRetry = function(message, retryCount, successCb, failedCb) {
  var retry = 0;
  var success = function(e) {
    if (typeof successCb == "function") {
      successCb(e);
    }
  };
  var failed = function(e) {
    retry++;
    if (retry < retryCount) {
      Pebble.sendAppMessage(message, success, failed);
    }
    else {
      if (typeof failedCb == "function") {
        failedCb(e);
      }
    }
  };
  Pebble.sendAppMessage(message, success, failed);
};

function fetchWeather(latitude, longitude) {
  console.log("Fetch Weather");
  var response;
  var req = new XMLHttpRequest();
  req.open('GET', "http://www.mirz.com/Chunk2/Yahoo-x.php?" +
    "lat=" + latitude + "&long=" + longitude + "&v=24&units=" + UnitsToString(mConfig.units), true);
    console.log("http://www.mirz.com/Chunk2/Yahoo-x.php?" +
      "lat=" + latitude + "&long=" + longitude + "&v=24&units=" + UnitsToString(mConfig.units));
  req.onload = function(e) {
    if (req.readyState == 4) {
      if(req.status == 200) {
        console.log(JSON.stringify(req.responseText));
        response = JSON.parse(req.responseText);
        var temperature, high, low, code;
        if (response) {
          var weatherResult = response;
          temperature = weatherResult.temp;
          code = weatherResult.code;
          high = weatherResult.high;
          low = weatherResult.low;

          Pebble.sendAppMessageWithRetry({
            "temperature": temperature,
            "icon": code,
            "high": high,
            "low": low
            }, 10);
        }

      } else {
        //console.log("Weather Error");
      }
    }
  };
  req.send(null);
}

function locationSuccess(pos) {
    console.log("JS locationSuccess()");
    var coordinates = pos.coords;
    fetchWeather(coordinates.latitude, coordinates.longitude);
}

function locationError(err) {
  console.warn('JS locationError(' + err.code + '): ' + err.message);
  Pebble.sendAppMessageWithRetry({
    "temperature": 999,
    "icon": 48,
    "high": 0,
    "low": 0
  }, 10);
}

function saveLocalData(config) {
  localStorage.setItem("style", parseInt(config.style));
  localStorage.setItem("bluetoothvibe", parseInt(config.bluetoothvibe));
  localStorage.setItem("hourlyvibe", parseInt(config.hourlyvibe));
  localStorage.setItem("units", parseInt(config.units));
  localStorage.setItem("blink", parseInt(config.blink));
  localStorage.setItem("dateformat", parseInt(config.dateformat));

  localStorage.setItem("gpslat", config.gpslat===null?'':config.gpslat);
  localStorage.setItem("gpslon", config.gpslon===null?'':config.gpslon);

  loadLocalData();
}
function loadLocalData() {
  mConfig.style = parseInt(localStorage.getItem("style"));
  mConfig.bluetoothvibe = parseInt(localStorage.getItem("bluetoothvibe"));
  mConfig.hourlyvibe = parseInt(localStorage.getItem("hourlyvibe"));
  mConfig.units = parseInt(localStorage.getItem("units"));
  mConfig.blink = parseInt(localStorage.getItem("blink"));
  mConfig.dateformat = parseInt(localStorage.getItem("dateformat"));
  mConfig.configureUrl = "http://www.mirz.com/Chunk2/index2.5.html";
  mConfig.gpslat = localStorage.getItem("gpslat");
  mConfig.gpslon = localStorage.getItem("gpslon");

    if(isNaN(mConfig.style)) {
        mConfig.style = 1;
    }
    if(isNaN(mConfig.bluetoothvibe)) {
        mConfig.bluetoothvibe = 1;
    }
    if(isNaN(mConfig.hourlyvibe)) {
        mConfig.hourlyvibe = 0;
    }
    if(isNaN(mConfig.units)) {
        mConfig.units = 1;
    }
    if(isNaN(mConfig.blink)) {
        mConfig.blink = 1;
    }
    if(isNaN(mConfig.dateformat)) {
        mConfig.dateformat = 0;
    }

}
function returnConfigToPebble() {
    Pebble.sendAppMessageWithRetry({
      "style":parseInt(mConfig.style),
      "bluetoothvibe":parseInt(mConfig.bluetoothvibe),
      "hourlyvibe":parseInt(mConfig.hourlyvibe),
      "units":parseInt(mConfig.units),
      "blink":parseInt(mConfig.blink),
      "dateformat":parseInt(mConfig.dateformat)
    }, 10);
    getWeather();
}
function UnitsToString(unit) {
  if(unit===0) {
    return "f";
  }
  return "c";
}

function getWeather() {
	if(mConfig.gpslat!=='' && mConfig.gpslon!=='') {
		console.log("used fixed gps " + mConfig.gpslat);
		fetchWeather(mConfig.gpslat, mConfig.gpslon);
	}
	else {
		console.log("used auto gps");
		navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
	}

}

Pebble.addEventListener("ready", function(e) {
  console.log('ready');
  loadLocalData();
  returnConfigToPebble();
});


Pebble.addEventListener("appmessage", function(e) {
  getWeather();
});

Pebble.addEventListener("showConfiguration", function(e) {
	Pebble.openURL(mConfig.configureUrl);
});

Pebble.addEventListener("webviewclosed", function(e) {
  if (e.response) {
    var config = JSON.parse(decodeURIComponent(e.response));
    saveLocalData(config);
    returnConfigToPebble();
  }
});
