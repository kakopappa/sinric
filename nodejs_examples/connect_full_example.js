// By Manos Nikiforakis

const WebSocket = require('ws')
const token = 'Your api key here'
const thermostatId = '5xxxxxxxxxxxxxxxxxxxxxx'
const options = {
  headers: {
    // needs node.js version >= 5.10.0 for this to work as Buffer.from() is added at that milestone!
    'Authorization': Buffer.from('apikey:' + token).toString('base64')
  }
}



 // Keep live and detect disconnection
function heartbeat() {
  clearTimeout(this.pingTimeout);

  this.pingTimeout = setTimeout(() => {
    console.log("No Connection. Killing node...")
    this.terminate();
  }, 30000 + 1000);
}

const ws = new WebSocket('ws://iot.sinric.com', options)


ws.on('open', heartbeat);
ws.on('ping', heartbeat);
ws.on('close', function clear() {
  clearTimeout(this.pingTimeout);
});

ws.on('open', function open () {
  console.log('Connected. waiting for commands..')
})

ws.on('message', function incoming (data) {
  console.log(data)
  var jsonData = JSON.parse(data)
  if (jsonData.deviceId === thermostatId) {
    if (jsonData.action === 'setPowerState') {
      // command: "Alexa turn ON thermostat"
      // {"deviceId":"5xxxxxxxxxxxxxxxxxxxxxx","action":"setPowerState","value":"ON"}
      // command: "Alexa turn OFF thermostat"
      // {"deviceId":"5xxxxxxxxxxxxxxxxxxxxxx","action":"setPowerState","value":"OFF"}
      if (jsonData.value === 'ON') {
        console.log('Turning ON thermostat...')
      } else {
        console.log('Turning OFF thermostat...')
      }
    } else {
      if (jsonData.action === 'SetTargetTemperature') {
        // command: "Alexa set thermostat to 20"
        // {"deviceId":"5xxxxxxxxxxxxxxxxxxxxxx","action":"SetTargetTemperature","value":{"targetSetpoint":{"value":20,"scale":"CELSIUS"}}}
        var targetData = jsonData.value
        var targetSetpoint = targetData.targetSetpoint
        var value = targetSetpoint.value
        var scale = targetSetpoint.scale
        console.log('thermostat is set to ' + value + ' ' + scale)
      } else {
        if (jsonData.action === 'AdjustTargetTemperature') {
          // RAISE TEMPERATURE:
          // command: "Alexa make thermostat warmer"
          // {"deviceId":"5xxxxxxxxxxxxxxxxxxxxxx","action":"AdjustTargetTemperature","value":{"targetSetpointDelta":{"value":1,"scale":"CELSIUS"}}}
          // command: "Alexa increase thermostat 2 degrees"
          // {"deviceId":"5xxxxxxxxxxxxxxxxxxxxxx","action":"AdjustTargetTemperature","value":{"targetSetpointDelta":{"value":2,"scale":"CELSIUS"}}}
          // LOWER TEMPERATURE
          // command: "Alexa make thermostat cooler"
          // {"deviceId":"5xxxxxxxxxxxxxxxxxxxxxx","action":"AdjustTargetTemperature","value":{"targetSetpointDelta":{"value":-1,"scale":"CELSIUS"}}}
          // command: "Alexa decrease thermostat 3 degrees"
          // {"deviceId":"5xxxxxxxxxxxxxxxxxxxxxx","action":"AdjustTargetTemperature","value":{"targetSetpointDelta":{"value":-3,"scale":"CELSIUS"}}}
          var adjustTargetData = jsonData.value
          var targetSetpointDelta = adjustTargetData.targetSetpointDelta
          var deltaValue = targetSetpointDelta.value
          var deltaScale = targetSetpointDelta.scale
          console.log('thermostat is changing by ' + deltaValue + ' ' + deltaScale)
        } else {
          if (jsonData.action === 'SetThermostatMode') {
            // command: "Alexa set thermostat mode to AUTO"
            // other possible modes are: "COOL", "ECO", "HEAT" and "OFF"
            // {"deviceId":"5xxxxxxxxxxxxxxxxxxxxxx","action":"SetThermostatMode","value":{"thermostatMode":{"value":"AUTO"}}}
            var thermostatModeData = jsonData.value
            var thermostatMode = thermostatModeData.thermostatMode
            var modeValue = thermostatMode.value
            console.log('thermostat mode is set to ' + modeValue)
          }
        }
      }
    }
  }
})

