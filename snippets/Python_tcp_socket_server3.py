#!/usr/bin/env python

import socket               # Import socket module

port = 12345
host = socket.gethostname()     # Get local machine name

s = socket.socket()

s.bind((host, port))         # Bind to the port
print 'Server started! Waiting for clients...'

s.listen(5)                         # Now wait for client connection.

try:
    c, addr = s.accept()     # Establish connection with client.
    print 'Got connection from', addr
    while True:
        msg = c.recv(1024)
        if not msg:
            break
        print "[Received from %s] %s" % (addr, msg)
        # msg = raw_input('SERVER >> ')
        # c.send(msg);
finally:
    c.close()
