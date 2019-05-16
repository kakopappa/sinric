from sinric import Sinric
from time import sleep
import json
apiKey = 'Replace with your api key'  #https://sinric.com



def deviceAction(value,deviceName):
    if value == "ON":
        print(deviceName+' : ON')
    else:
        print(deviceName+' : OFF')

def selectDevice(deviceId,action,value):
    if deviceId == "5c**********":  #Replace with your deviceId
        deviceAction(value,'Bulb')

    elif deviceId == "5c12c********": #Replace with your deviceId
        deviceAction(value,'Fan')

if __name__ == '__main__':
    obj  = Sinric(apiKey)
    while True:
        response = obj.initialize()
        data =  json.loads(response)
        print(data)
        deviceId = data['deviceId']
        action = data['action']
        value = data['value']
        selectDevice(deviceId,action,value)
        sleep(2)

