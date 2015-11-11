from config import Config
from websocket import Websocket


def bootstrap():
    """
    Initiates all components that are needed after arduino-yun's linuino
    has started.
    """
    socket = Websocket(
        Config.get_socket_host(),
        Config.get_socket_port()
    )


def main():
    bootstrap()


if __name__ == "__main__":
    main()
