const WebSocket = require('ws');
const token = "your api key here";

const options = {
    headers: {
        "Authorization" : Buffer.from("apikey:" + token).toString('base64')
    }
};

const ws = new WebSocket('ws://iot.sinric.com', options);
 
ws.on('open', function open() {
   console.log("Connected. waiting for commands..");
});