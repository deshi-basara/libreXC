import trollius as asyncio

from autobahn.asyncio.websocket import WebSocketServerProtocol, \
    WebSocketServerFactory
from autobahn.websocket.http import HttpException
from security import Security
from config import Config


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

        received_cmd = payload.decode('utf8')

        # send test broadcast
        self.factory.broadcast('{"key":"value"}')

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
        for subscriber in self.subscribers:
            subscriber.sendMessage(json.encode("utf-8"))


class Websocket(object):
    """
    Initiates a websocket-server on the given host and port from .config.ini.
    All websocket-connections are managed inside WebsocketEvents-class.
    """

    def __init__(self, host, port):
        # @todo use wss for encrypted socket-transport
        socket_url = u"ws://{0}:{1}".format(host, port)

        # factory for creating instances of the WebsocketEvents-class
        factory = LibreSocketFactory(socket_url, debug=False)
        factory.protocol = LibreSocketEvents

        # create server socket inside the event loop
        self.loop = asyncio.get_event_loop()
        coroutine = self.loop.create_server(factory, '0.0.0.0', port)
        self.socket_server = self.loop.run_until_complete(coroutine)
        print("WebSocket started on: {0}".format(socket_url))

        try:
            self.loop.run_forever()
        except KeyboardInterrupt:
            pass
        finally:
            self.close()

    def close(self):
        self.socket_server.close()
        self.loop.close()
        print("WebSocket closed")

    def update(self, json):
        factory.broadcast(json)
