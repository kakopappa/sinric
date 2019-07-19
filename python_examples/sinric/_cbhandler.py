class CallBackHandler:
    def __init__(self, callbacks, logger):
        self.callbacks = callbacks
        self.logger = logger

    def handle_callbacks(self, jsn):
        if jsn.get("action") == "setPowerState":
            try:
                power_callback = self.callbacks.get("setPowerState")
                power_callback(jsn.get("deviceId"), jsn["value"])
            except Exception:
                self.logger.exception("Error Occurred")

        elif jsn.get("action") == "SetTargetTemperature":
            try:
                target_temperature_callback = self.callbacks.get("SetTargetTemperature")
                target_temperature_callback(
                    jsn.get("deviceId"),
                    jsn["value"]["targetSetPoint"]["value"],
                    jsn["value"]["targetSetPoint"]["scale"],
                )
            except Exception:
                self.logger.exception("Error Occurred")

        elif jsn.get("action") == "AdjustTargetTemperature":
            try:
                adjust_temperature_callback = self.callbacks.get(
                    "AdjustTargetTemperature"
                )
                adjust_temperature_callback(
                    jsn.get("deviceId"),
                    jsn["value"]["targetSetpointDelta"]["value"],
                    jsn["value"]["targetSetpointDelta"]["scale"],
                )
            except Exception:
                self.logger.exception("Error Occurred")

        elif jsn.get("action") == "SetThermostatMode":
            try:
                setmode_callback = self.callbacks.get("SetThermostatMode")
                setmode_callback(
                    jsn.get("deviceId"), jsn["value"]["thermostatMode"]["value"]
                )
            except Exception:
                self.logger.exception("Error Occurred")

        elif jsn.get("action") == "SetMute":
            try:
                set_mute_callback = self.callbacks.get("SetMute")
                set_mute_callback(jsn.get("deviceId"), jsn["value"])
            except Exception:
                self.logger.exception("Error Occurred")

        elif jsn.get("action") == "AdjustVolume":
            try:
                adjust_volume_callback = self.callbacks.get("AdjustVolume")
                adjust_volume_callback(
                    jsn.get("deviceId"),
                    jsn["value"]["volume"],
                    jsn["value"]["volumeDefault"],
                )
            except Exception:
                self.logger.exception("Error Occurred")

        elif jsn.get("action") == "ChangeChannel":
            try:
                change_channel_callback = self.callbacks.get("ChangeChannel")
                change_channel_callback(
                    jsn.get("deviceId"),
                    jsn["value"]["channel"],
                    jsn["value"]["channelMetadata"]["name"],
                )
            except Exception:
                self.logger.exception("Error Occurred")

        elif jsn.get("action") == "SkipChannels":
            try:
                skip_channel_callback = self.callbacks.get("SkipChannels")
                skip_channel_callback(jsn.get("deviceId"), jsn["value"]["channelCount"])
            except Exception:
                self.logger.exception("Error Occurred")

        elif jsn.get("action") == "Previous" or jsn.get("action") == "Play":
            try:
                previous_callback = self.callbacks.get("Previous")
                previous_callback(jsn.get("deviceId"), jsn.get("value"))
            except Exception:
                self.logger.exception("Error Occurred")

        elif jsn.get("action") == "SelectInput":
            try:
                select_input_callback = self.callbacks.get("SelectInput")
                select_input_callback(jsn.get("deviceId"), jsn["value"]["input"])
            except Exception:
                self.logger.exception("Error Occurred")
