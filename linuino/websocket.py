from threading import Thread
from autobahn.twisted.websocket import WebSocketServerProtocol, \
    WebSocketServerFactory
from twisted.internet import reactor
from autobahn.websocket.http import HttpException
from security import Security
from config import Config
from commands import Commands


class LibreSocketEvents(WebSocketServerProtocol):
    """
    Implementation of all available websocket-events with their handlers.
    """

    def onConnect(self, request):
        """
        Called whenever a new client connects to our websocket.
        Is used for authenticating newly connected clients.
        """
        # check if the user sent valid authentication attributes
        authenticated = Security.auth_status(
            request.headers,
            Config.get_user(),
            Config.get_password()
        )
        if not authenticated:
            # deny connection
            raise HttpException(401, "Invalid authentication data.")
        else:
            print("Authenticated: {0}".format(request.peer))
            self.factory.add_client(self)

    def onOpen(self):
        """
        Called whenever a newly authenticated client has opened his
        socket-connection.
        """
        pass

    def onMessage(self, payload, isBinary):
        """
        Called whenever a new message is sent by an authenticated client.
        """
        if isBinary:
            print("Binary message received: {0} bytes".format(len(payload)))
            return

        # validate received command
        try:
            received_cmd = payload.decode('utf8')
            cmd, value = Commands.validate_cmd(received_cmd)
        except Exception as error:
            self.factory.broadcast(str(error))
            return

        # hand valid cmd to arduino
        Commands.request_cmd(cmd, value)

    def onClose(self, wasClean, code, reason):
        """
        Called whenever a client closes his connection.
        """
        print("WebSocket connection closed: {0}".format(reason))
        self.factory.remove_client(self)


class LibreSocketFactory(WebSocketServerFactory):
    """
    WebSocketServerFactory with extended features for managing connected
    clients and broadcasts.
    """

    def __init__(self, socket_url, debug=False, debugCodePaths=False):
        WebSocketServerFactory.__init__(
            self,
            socket_url,
            debug=debug,
            debugCodePaths=debugCodePaths
        )
        self.subscribers = []

    def add_client(self, client):
        """
        Checks if a newly authenticated client is already a subscriber for
        our logging data. If not, the client will be added.
        """
        if client not in self.subscribers:
            self.subscribers.append(client)
            client_count = len(self.subscribers)
            print("Connected clients: {0} (+1)".format(client_count))

    def remove_client(self, client):
        """
        Removes a client from the subscriber-list.
        """
        if client in self.subscribers:
            self.subscribers.remove(client)
            client_count = len(self.subscribers)
            print("Connected clients: {0} (-1)".format(client_count))

    def broadcast(self, json):
        """
        Broadcasts a json-string to all subscibed clients.
        """
        print("broadcasting")
        for subscriber in self.subscribers:
            subscriber.sendMessage(json.encode("utf-8"))


class Websocket(Thread):
    """
    Initiates a websocket-server on the given host and port from .config.ini.
    All websocket-connections are managed inside WebsocketEvents-class.
    """

    def __init__(self, _host, _port, _data):
        super(Websocket, self).__init__()
        self.host = _host
        self.port = _port
        self.data = _data

        # @todo use wss for encrypted socket-transport
        self.socket_url = u"ws://{0}:{1}".format(self.host, self.port)
        self.factory = LibreSocketFactory(self.socket_url, debug=False)

    def run(self):
        # factory for creating instances of the WebsocketEvents-class
        self.factory.protocol = LibreSocketEvents
        try:
            reactor.listenTCP(int(self.port), self.factory)
            print("WebSocket-Thread: Started on {0}".format(self.socket_url))
            reactor.run(installSignalHandlers=0)
        except Exception as e:
            print(e)

    def close(self):
        self.factory.close()
        print("WebSocket closed")

    def update(self, _json):
        reactor.callFromThread(
            self.factory.broadcast,
            _json
        )
