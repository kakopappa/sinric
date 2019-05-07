#!/usr/bin/python
import websocket
import thread
import time
import base64
import ast
import RPi.GPIO as GP

BulbPin = 3 #set pin Number for device

FanPin = 5

GP.setmode(GP.BOARD)

GP.setup(BulbPin,GP.OUT) #declare pin as output

GP.setup(FanPin,GP.OUT) 

def deviceAction(Pin,value,deviceName):
    if value == "ON":
        print(deviceName+' : ON')
        GP.output(Pin,1)
    else:
        print(deviceName+' : OFF')
        GP.output(Pin,0)

def selectDevice(deviceId,action,value):
    if deviceId == "5c12c********":  #Replace with your deviceId
        deviceAction(BulbPin,value,'Bulb')

    elif deviceId == "5c12c********": #Replace with your deviceId
        deviceAction(FanPin,value,'Fan')

def on_message(ws, message):
    obj = ast.literal_eval(message)
    deviceId = obj['deviceId']
    action = obj['action']
    value = obj['value']
    selectDevice(deviceId,action,value)
    print(message)

def on_error(ws, error):
    print (error)

def on_close(ws):
    print "### closed ###"
    time.sleep(2)
    initiate()

def on_open(ws):
    print "### Initiating new websocket connection ###"
    
def initiate():
    websocket.enableTrace(True)
		
    ws = websocket.WebSocketApp("ws://iot.sinric.com",
		header={'Authorization:' +  base64.b64encode('apikey:[replace with your api key]')},
        on_message = on_message,
        on_error = on_error,
        on_close = on_close)
    ws.on_open = on_open

    ws.run_forever()

if __name__ == "__main__":
    initiate()
