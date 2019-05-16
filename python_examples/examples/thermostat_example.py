from time import sleep
# import ast
import json
from sinric import Sinric

apiKey = 'Replace with your api key'


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

    # For Thermostat
    # {"deviceId": xxxx, "action": "setPowerState", value: "ON"} // https://developer.amazon.com/docs/device-apis/alexa-thermostatcontroller.html
    # {"deviceId": xxxx, "action": "SetTargetTemperature", value: "targetSetpoint": { "value": 20.0, "scale": "CELSIUS"}} // https://developer.amazon.com/docs/device-apis/alexa-thermostatcontroller.html#settargettemperature
    # {"deviceId": xxxx, "action": "AdjustTargetTemperature", value: "targetSetpointDelta": { "value": 2.0, "scale": "FAHRENHEIT" }} // https://developer.amazon.com/docs/device-apis/alexa-thermostatcontroller.html#adjusttargettemperature
    # {"deviceId": xxxx, "action": "SetThermostatMode", value: "thermostatMode" : { "value": "COOL" }} // https://developer.amazon.com/docs/device-apis/alexa-thermostatcontroller.html#setthermostatmode


if __name__ == '__main__':
    obj = Sinric(apiKey)
    while True:
        response = obj.initialize()
        data = json.loads(response)
        deviceId = data['deviceId']
        action = data['action']
        value = data['value']
        selectionAction(deviceId, action, value)
        sleep(2)
