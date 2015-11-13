import Queue


class Data(object):
    def __init__(self):
        self.queue = Queue.Queue(maxsize=100)
        self.observers = []

    def add_data(self, json):
        self.queue.put(json)
        self.notify(json)

    def add_observer(self, observer):
        self.observers.append(observer)

    def notify(self, json):
        for observer in self.observers:
            observer.update(json)
