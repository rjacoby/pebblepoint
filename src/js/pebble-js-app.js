var initialized = false;
var config = {};

Pebble.addEventListener("ready", function() {
  console.log("ready called!");
  initialized = true;
});

Pebble.addEventListener("showConfiguration", function() {
  console.log("showing configuration");
  Pebble.openURL('https://dl.dropboxusercontent.com/u/115264/pebble/ppt/configurable.html');
});

Pebble.addEventListener("webviewclosed", function(e) {
  console.log("configuration closed");
  // webview closed
  var options = JSON.parse(decodeURIComponent(e.response));
  console.log("Options = " + JSON.stringify(options));
  config = options;
});

function sendPowerPointCommand(command, callback, errorCallback) {
    var req = new XMLHttpRequest();
    // TODO: test if we're configured
    url = "http://" + config.address + ":" + config.port + "/go/" + command;
    console.log("POSTing to: " + url);
    req.open("POST", url, true);
    req.responseType = "arraybuffer";
    req.onload = function(e) {
      console.log("loaded");
      var body = req.response;
      if(req.status == 200 && body) {
        responseData = JSON.parse(body);
        console.log("body", responseData);
        callback(body); //???
      }
      else {
        errorCallback("Request status is " + req.status);
      }
    };
    req.onerror = function(e) {
      errorCallback(e);
    };
    req.send(null);
}

Pebble.addEventListener("appmessage",
  function(e) {
    console.log("Received message: " + e.payload);
    // TODO: call the send helper with a munged payload
  }
);
