from threading import Thread
from libs.bridgeclient import BridgeClient

import Queue


class Data(Thread):
    def __init__(self):
        super(Data, self).__init__()
        self.queue = Queue.Queue(maxsize=100)
        self.observers = []

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
            # log_data = bridge.get("json")
            log_data = "running"
            # print(log_data)
            if log_data:
                print(log_data)
                # self.add_data(log_data)"""

    def notify(self, json):
        print("notify observer")
        for observer in self.observers:
            observer.update(json)
