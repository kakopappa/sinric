import websocket
from base64 import b64encode as enc


class Sinric:
    def __init__(self, apikey):
        self.apikey = apikey.encode('ascii')

    def initialize(self):
        websocket.enableTrace(True)
        ws = websocket.create_connection('ws://iot.sinric.com',
                                         header={'Authorization:' + enc(b'apikey:'+self.apikey).decode('ascii')})
        response = ws.recv()
        ws.close()
        return response
