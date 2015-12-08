import Queue
import time

from threading import Thread
from libs.bridgeclient import BridgeClient
from config import Config


class Data(Thread):
    def __init__(self):
        super(Data, self).__init__()
        self.queue = Queue.Queue(maxsize=100)
        self.observers = []
        self.socket_timeout = Config.get_socket_timeout()

    def add_data(self, json):
        print("data added")
        self.queue.put(json)
        self.notify(json)

    def add_observer(self, observer):
        print("observer added")
        self.observers.append(observer)

    def run(self):
        bridge = BridgeClient()

        while True:
            # listen for bridge-commands
            try:
                log_data = bridge.get("json")

                if log_data:
                    print(log_data)
                    # self.add_data(log_data)
            except Exception as e:
                # print exception and let Thread sleep
                print("Data-Thread Error: {}".format(e))
                time.sleep(self.socket_timeout)

    def notify(self, json):
        print("notify observer")
        for observer in self.observers:
            observer.update(json)
