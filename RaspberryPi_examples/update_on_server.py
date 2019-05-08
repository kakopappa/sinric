import websocket
import threading
import time
import base64
import ast
import json
from collections import OrderedDict

apiKey =  'replace with your api key'
deviceId = 'replace with your device ID'

def getJSON(deviceId,value):
    temp = OrderedDict([('deviceId',deviceId),('action','setPowerState'),('value',value)])
    print('Sending : '+json.dumps(temp))
    return json.dumps(temp)

if __name__ == "__main__":
    websocket.enableTrace(True)
    ws = websocket.create_connection('ws://iot.sinric.com',header={'Authorization:' + base64.b64encode('apikey:' + apiKey)})
    message = getJSON(deviceId,'OFF')
    ws.send(message)
    print("Sent")
    print("Receiving...")
    result = ws.recv()
    print("Received '%s'" % result)
ws.close()