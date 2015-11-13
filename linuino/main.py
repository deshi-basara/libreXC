from config import Config
from data import Data
from logger import Logger
from websocket import Websocket


def main():
    """
    Initiates all components that are needed after arduino-yun's linuino
    has started.
    """
    # initiate data-object
    data = Data()
    logger = Logger("default", data)

    # initiate needed threads
    socket_thread = Websocket(
        Config.get_socket_host(),
        Config.get_socket_port(),
        data
    )
    # socket_thread.daemon = True
    socket_thread.start()

    # add observers to data-object
    data.add_observer(logger)
    data.add_observer(socket_thread)

    data.add_data("{key:value}")


if __name__ == "__main__":
    main()
