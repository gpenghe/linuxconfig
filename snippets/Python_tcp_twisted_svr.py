#!/usr/bin/env python

from twisted.internet import protocol, reactor, endpoints

class Echo(protocol.Protocol):
    def dataReceived(self, data):
        self.transport.write('--->' + data)
    def __init__(self, factory):
        self.factory = factory

    def connectionMade(self):
        self.factory.numProtocols = self.factory.numProtocols + 1
        if self.factory.numProtocols >1:
            self.transport.write("Server is busy. Try again later.\n")
            self.transport.loseConnection()

    def connectionLost(self, reason):
        self.factory.numProtocols = self.factory.numProtocols - 1

class EchoFactory(protocol.Factory):
    def __init__(self):
        self.numProtocols = 0
    def buildProtocol(self, addr):
        return Echo(self)

endpoint = endpoints.serverFromString(reactor, "tcp:1234")
factory = EchoFactory()
endpoint.listen(factory)
reactor.run()
