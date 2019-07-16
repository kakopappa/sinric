from ._sinricsocket import SinricSocket
from threading import Thread


class Sinric:
    def __init__(self, apiKey, callbacks):
        self.apiKey = apiKey
        self.callbacks = callbacks
        self.socket_handler = SinricSocket(self.apiKey, self.callbacks)

    def socket_listen(self):
        self.socket_handler.listen()

    def handle(self):
        t1 = Thread(target=self.socket_handler.handle_queue)
        t1.setDaemon(True)
        t1.start()
        self.socket_listen()
