import Queue
import time
import serial

from threading import Thread
from config import Config


class Data(Thread):
    def __init__(self):
        super(Data, self).__init__()
        self.queue = Queue.Queue(maxsize=100)
        self.observers = []
        self.socket_timeout = Config.get_socket_timeout()

    def add_data(self, json):
        self.queue.put(json)
        self.notify(json)

    def add_observer(self, observer):
        self.observers.append(observer)

    def run(self):
        connection = serial.Serial(port=Config.get_tty_port(), baudrate=57600)

        while True:
            # listen for bridge-commands
            try:
                line = connection.readline()

                if line:
                    print(line)
                    self.add_data(line)
            except Exception as e:
                # print exception and let Thread sleep
                print("Data-Thread Error: {}".format(e))
                time.sleep(self.socket_timeout)

    """def run(self):
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
                time.sleep(self.socket_timeout)"""

    def notify(self, json):
        print("notify observer")
        for observer in self.observers:
            observer.update(json)
