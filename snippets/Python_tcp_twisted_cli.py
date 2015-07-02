#!/usr/bin/env python
from twisted.internet.protocol import Protocol
from twisted.internet import reactor
from twisted.internet.endpoints import TCP4ClientEndpoint, connectProtocol

from sys import stdout

class Echo(Protocol):
    def dataReceived(self, data):
        # stdout.write(data)
        print data
    def connectionMade(self):
        self.transport.write("Hello server, I am the client!\r\n")
        self.transport.loseConnection()


class Greeter(Protocol):
    def connectionMade(self):
        self.transport.write("Hello server, I am the client\n")
    def dataReceived(self, data):
        # stdout.write(data)
        print data
    def sendMessage(self, msg):
        self.transport.write("MESSAGE %s\n" % msg)

def gotProtocol(p):
    p.sendMessage("Hello")
    reactor.callLater(1, p.sendMessage, "This is sent in a second")
    reactor.callLater(2, p.transport.loseConnection)

endpoint = TCP4ClientEndpoint(reactor, "localhost", 1234)
d = connectProtocol(endpoint, Greeter())
d.addCallback(gotProtocol)
reactor.run()
