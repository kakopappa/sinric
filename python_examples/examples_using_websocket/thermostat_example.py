import websocket
import threading
import time
import base64
import json


def onSetPowerState(deviceId, value):
    print('setPowerState : value = ' + value)


def onSetTargetTemperature(deviceId, value):
    tempValue = value['targetSetpoint']
    temperatureValue = tempValue['value']
    scale = tempValue['scale']
    if scale == 'CELSIUS':
        print('Value set to : ' + str(temperatureValue) + '  ' + scale)

    elif scale == 'FAHRENHEIT':
        print('Value set to : ' + str(temperatureValue) + '  ' + scale)


def onAdjustTargetTemperature(deviceId, value):
    tempValue = value['targetSetpointDelta']
    temperatureValue = tempValue['value']
    scale = tempValue['scale']

    if scale == 'CELSIUS':
        print('Value adjusted to : ' + str(temperatureValue) + '  ' + scale)

    elif scale == 'FAHRENHEIT':
        print('Value adjusted to : ' + str(temperatureValue) + '  ' + scale)


def onSetThermostatMode(deviceId, value):
    tempValue = value['thermostatMode']
    mode = tempValue['value']
    print('Mode set to ' + mode)


def selectionAction(deviceId, action, value):
    if action == 'setPowerState':
        onSetPowerState(deviceId, value)
    elif action == 'SetTargetTemperature':
        onSetTargetTemperature(deviceId, value)
    elif action == 'AdjustTargetTemperature':
        onAdjustTargetTemperature(deviceId, value)
    elif action == 'SetThermostatMode':
        onSetThermostatMode(deviceId, value)
    elif action == 'test':
        print('Received a test command')


def on_message(ws, message):  # Callback function on successfull response from server
    obj = json.loads(message)
    deviceId = obj['deviceId']
    action = obj['action']
    value = obj['value']
    selectionAction(deviceId, action, value)
    # print(message)      #Prints the JSON response 

    # For Thermostat
    # {"deviceId": xxxx, "action": "setPowerState", value: "ON"} // https://developer.amazon.com/docs/device-apis/alexa-thermostatcontroller.html
    # {"deviceId": xxxx, "action": "SetTargetTemperature", value: "targetSetpoint": { "value": 20.0, "scale": "CELSIUS"}} // https://developer.amazon.com/docs/device-apis/alexa-thermostatcontroller.html#settargettemperature
    # {"deviceId": xxxx, "action": "AdjustTargetTemperature", value: "targetSetpointDelta": { "value": 2.0, "scale": "FAHRENHEIT" }} // https://developer.amazon.com/docs/device-apis/alexa-thermostatcontroller.html#adjusttargettemperature
    # {"deviceId": xxxx, "action": "SetThermostatMode", value: "thermostatMode" : { "value": "COOL" }} // https://developer.amazon.com/docs/device-apis/alexa-thermostatcontroller.html#setthermostatmode


def on_error(ws, error):
    print(error)


def on_close(ws):
    print('### closed ###')
    time.sleep(2)
    initiate()


def on_open(ws):
    print('### Initiating new websocket connection ###')


def initiate():
    websocket.enableTrace(True)

    ws = websocket.WebSocketApp('ws://iot.sinric.com',
                                header={
                                    'Authorization:' + base64.b64encode('apikey:[replace with your api key ]')},
                                on_message=on_message,
                                on_error=on_error,
                                on_close=on_close)
    ws.on_open = on_open

    ws.run_forever()


if __name__ == '__main__':
    initiate()
