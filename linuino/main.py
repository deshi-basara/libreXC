import signal
import sys
import time

from config import Config
from data import Data
from logger import Logger
from websocket import Websocket


def interrupt(signal, frame):
    """
    Callback method used by python's signal-modul.
    If ctrl+c is pressed, main.py will be closed.
    """
    print("... libreXC-Daemon has been terminated.")
    sys.exit(0)


def main():
    """
    Initiates all components that are needed after arduino-yun's linuino
    has started.
    """
    threads = []

    # initiate all needed threads
    data_thread = Data()
    socket_thread = Websocket(
        Config.get_socket_host(),
        Config.get_socket_port(),
        data_thread
    )
    threads.append(data_thread)
    threads.append(socket_thread)

    # start all needed threads
    for thread in threads:
        thread.daemon = True
        thread.start()

    # add observers to data-object
    logger = Logger("default", data_thread)
    data_thread.add_observer(logger)
    data_thread.add_observer(socket_thread)

    # keep main() alive to prevent an exit of our
    # deamonized threads
    while True:
        time.sleep(1)


if __name__ == "__main__":
    signal.signal(signal.SIGINT, interrupt)
    main()
