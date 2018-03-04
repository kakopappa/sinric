const WebSocket = require('ws');
const token = "Your api key here";

const options = {
    headers: {
        "Authorization" : Buffer.from("apikey:" + token).toString('base64')
    }
};

const ws = new WebSocket('ws://iot.sinric.com', options);
 
ws.on('open', function open() {
   console.log("Connected. waiting for commands..");
});

ws.on('message', function incoming(data) {
   console.log("Request : " + data)
   let cmdObj = JSON.parse(data);

   if(cmdObj.action == "setPowerState") {
       if(cmdObj.value === "ON") {
           console.log("Turn on...")
       } else {
           console.log("Turn off...")
       } 
   }

});