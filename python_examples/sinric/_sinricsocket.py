import json
from base64 import b64encode as enc

import websocket
from loguru import logger

from ._cbhandler import CallBackHandler
from ._mainqueue import queue

logger.add("{}.log".format(__name__), rotation="10 MB")


class SinricSocket:
    def __init__(self, apikey, callbacks):
        self.apikey = apikey.encode("ascii")
        self.callbacks = callbacks
        self.logger = logger
        self.callback_handler = CallBackHandler(self.callbacks, self.logger)

    def on_message(self, response):
        self.logger.info(response)
        queue.put(json.loads(response))

    def on_error(self, error):
        self.logger.error(error)

    def on_close(self):
        self.logger.info("### closed ###")

    def on_open(self):
        # print('open')
        pass

    def listen(self):
        websocket.enableTrace(False)
        ws = websocket.WebSocketApp(
            "ws://iot.sinric.com",
            header={"Authorization:" + enc(b"apikey:" + self.apikey).decode("ascii")},
            on_message=self.on_message,
            on_error=self.on_error,
            on_close=self.on_close,
        )

        ws.on_open = self.on_open
        ws.run_forever()

    def handle_queue(self):
        while True:
            while queue.qsize() > 0:
                self.callback_handler.handle_callbacks(queue.get())
