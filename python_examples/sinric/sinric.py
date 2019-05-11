import websocket
import threading
import time
import base64
import json
from collections import OrderedDict



class Sinric:
    def __init__(self, apikey):
        self.apikey = apikey

    def initialize(self):
        websocket.enableTrace(True)
        ws = websocket.create_connection('ws://iot.sinric.com',
                                         header={'Authorization:' + base64.b64encode('apikey:' + self.apikey)})
        # print("Sent")
        response = ws.recv()
        ws.close()
        return response
