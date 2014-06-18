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

function sendPowerPointCommand(command, callback, errorCallback) {
    var req = new XMLHttpRequest();
    // Use the JS toolkit's _own_ localStorage - it's not shared with the
    // config view's browser scoped localStorage.
    config = JSON.parse(window.localStorage.pebblepoint_options); // TODO: test for sanity?
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
        callback(responseData); //???
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
    var command = e.payload[0];
    console.log("Command: " + command);
    // TODO: call the send helper with a munged payload
    sendPowerPointCommand(command);
  }
);
