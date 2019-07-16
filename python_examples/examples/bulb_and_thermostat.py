from sinric import Sinric

apiKey = 'Api Key'


def power_state(deviceId, state):
    print(deviceId, state)


def set_target_temp(deviceId, value, scale):
    print(deviceId, value, scale)


def adjust_target_temp(deviceId, value, scale):
    print(deviceId, value, scale)


def set_thermostat_mode(deviceId, value):
    print(deviceId, value)


callbacks = {
    'setPowerState': power_state,
    'SetTargetTemperature': set_target_temp,
    'AdjustTargetTemperature': adjust_target_temp,
    'SetThermostatMode': set_thermostat_mode,
}

if __name__ == '__main__':
    ob = Sinric(apiKey, callbacks)
    ob.handle()
