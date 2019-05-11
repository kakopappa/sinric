import websocket
import threading
import time
import base64
import ast
import json
from collections import OrderedDict

# USE ONLY If status changed. DO NOT CALL THIS IN loop() and overload the server. 

apiKey =  'replace with your api key'
deviceId = 'replace with your device ID'

def getJSON(deviceId,value):
    temp = OrderedDict([('deviceId',deviceId),('action','setPowerState'),('value',value)]) #Used OrderedDict function because dict() alerts the input json
    print('Sending : '+json.dumps(temp))
    return json.dumps(temp)  #Convert OrderedDict object to json string

if __name__ == "__main__":
    websocket.enableTrace(True)
    ws = websocket.create_connection('ws://iot.sinric.com',header={'Authorization:' + base64.b64encode('apikey:' + apiKey)})
    message = getJSON(deviceId,value = 'OFF') #set value to turn ON/OFF your device
    ws.send(message)
    print("Sent")
ws.close()