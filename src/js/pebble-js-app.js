var initialized = false;

Pebble.addEventListener("ready", function() {
  console.log("ready called!");
  console.log("stored options", window.localStorage.pebblepoint_options);
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
  jso = JSON.stringify(options);
  console.log("Options = " + jso);
  if(typeof window.localStorage !== "undefined") {
    window.localStorage.pebblepoint_options = jso;
  }
});

// Success callback
function sendSuccessToPebble(jsonData) {
  console.log("success callback", jsonData);
  Pebble.sendAppMessage({
    "success": 1,
    "message": "Navigated OK",
    "slideIndex": jsonData.slideIndex,
    "slideTotal": jsonData.slideTotal
  });
}

// Failure callback
function sendFailureToPebble(jsonData) {
  console.log("failure callback", jsonData);
  Pebble.sendAppMessage({
    "success": 0,
    "message": jsonData.errorMessage || "Check configuration"
  });
}

function sendPowerPointCommand(command, callback, errorCallback) {
    var req = new XMLHttpRequest();
    // Use the JS toolkit's _own_ localStorage - it's not shared with the
    // config view's browser scoped localStorage.
    config = JSON.parse(window.localStorage.pebblepoint_options);
    var url = "http://" + config.address + ":" + config.port + "/go/" + command;
    console.log("POSTing to: " + url);
    req.open("POST", url, true);
    req.responseType = "arraybuffer";
    req.onload = function(e) {
      console.log("loaded");
      var body = req.response;
      if(req.status == 200 && req.responseText) {
        console.log(req.responseText);
        var responseData = JSON.parse(req.responseText);
        console.log("responseData", responseData);
        if (responseData.success) {
          callback(responseData);
        } else {
          errorCallback(responseData);
        }
      } else {
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
    var command = e.payload.command;
    console.log("Command: " + command);
    sendPowerPointCommand(command, sendSuccessToPebble, sendFailureToPebble);
  }
);
