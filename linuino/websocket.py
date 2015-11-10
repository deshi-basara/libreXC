import trollius as asyncio

from autobahn.asyncio.websocket import WebSocketServerProtocol, \
    WebSocketServerFactory


class WebsocketEvents(WebSocketServerProtocol):

    def onConnect(self, request):
        print("Client connecting: {0}".format(request.peer))

    def onOpen(self):
        print("WebSocket connection open.")

    def onMessage(self, payload, isBinary):
        if isBinary:
            print("Binary message received: {0} bytes".format(len(payload)))
        else:
            print("Text message received: {0}".format(payload.decode('utf8')))

        # echo back message verbatim
        self.sendMessage(payload, isBinary)

    def onClose(self, wasClean, code, reason):
        print("WebSocket connection closed: {0}".format(reason))


def start():

    # factory for creating instances of the WebsocketEvents-class
    factory = WebSocketServerFactory(u"ws://127.0.0.1:9000", debug=False)
    factory.protocol = WebsocketEvents

    # create server socket inside the event loop
    loop = asyncio.get_event_loop()
    coroutine = loop.create_server(factory, '0.0.0.0', 9000)
    socket_server = loop.run_until_complete(coroutine)
    print("WebSocket started on Port: {0}".format(9000))

    try:
        loop.run_forever()
    except KeyboardInterrupt:
        pass
    finally:
        socket_server.close()
        loop.close()
        print("WebSocket closed")
