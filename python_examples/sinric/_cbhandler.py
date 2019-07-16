
class CallBackHandler:
    def __init__(self, callbacks):
        self.callbacks = callbacks

    def handle_callbacks(self, jsn):
        if jsn['action'] == 'setPowerState':
            try:
                power_callback = self.callbacks['setPowerState']
                power_callback(jsn['deviceId'], jsn['value'])
            except Exception as e:
                print(e)
        elif jsn['action'] == 'SetTargetTemperature':
            try:
                target_temperature_callback = self.callbacks['SetTargetTemperature']
                target_temperature_callback(jsn['deviceId'], jsn['value']['targetSetPoint']['value'],
                                            jsn['value']['targetSetPoint']['scale'])
            except Exception as e:
                print(e)
        elif jsn['action'] == 'AdjustTargetTemperature':
            try:
                adjust_temperature_callback = self.callbacks['AdjustTargetTemperature']
                adjust_temperature_callback(jsn['deviceId'], jsn['value']['targetSetpointDelta']['value'],
                                            jsn['value']['targetSetpointDelta']['scale'])
            except Exception as e:
                print(e)
        elif jsn['action'] == 'SetThermostatMode':
            try:
                setmode_callback = self.callbacks['SetThermostatMode']
                setmode_callback(jsn['deviceId'], jsn['value']['thermostatMode']['value'])
            except Exception as e:
                print(e)
