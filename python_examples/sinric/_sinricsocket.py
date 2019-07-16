import websocket
from base64 import b64encode as enc
import json
from ._mainqueue import queue
from ._cbhandler import CallBackHandler


class SinricSocket:
    def __init__(self, apikey, callbacks):
        self.apikey = apikey.encode('ascii')
        self.callbacks = callbacks
        self.callback_handler = CallBackHandler(self.callbacks)

    def on_message(self, response):
        print(response)
        queue.put(json.loads(response))

    def on_error(self, error):
        print(error)

    def on_close(self):
        print("### closed ###")

    def on_open(self):
        # print('open')
        pass

    def listen(self):
        websocket.enableTrace(False)
        ws = websocket.WebSocketApp('ws://iot.sinric.com',
                                    header={'Authorization:' + enc(b'apikey:' + self.apikey).decode('ascii')},
                                    on_message=self.on_message,
                                    on_error=self.on_error,
                                    on_close=self.on_close)

        ws.on_open = self.on_open
        ws.run_forever()

    def handle_queue(self):
        while True:
            while queue.qsize() > 0:
                self.callback_handler.handle_callbacks(queue.get())
