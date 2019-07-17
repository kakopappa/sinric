from loguru import logger

from sinric import Sinric

apiKey = "Api Key"


def power_state(deviceId, state):
    logger.info("{} {}".format(deviceId, state))


callbacks = {"setPowerState": power_state}

if __name__ == "__main__":
    logging.basicConfig(filename="myapp.log", level=logging.INFO)
    ob = Sinric(apiKey, callbacks)
    ob.handle()
