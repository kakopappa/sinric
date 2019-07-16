from sinric import Sinric

apiKey = 'Api Key'


def power_state(deviceId, state):
    print(deviceId, state)


callbacks = {
    'setPowerState': power_state,
}

if __name__ == '__main__':
    ob = Sinric(apiKey, callbacks)
    ob.handle()
