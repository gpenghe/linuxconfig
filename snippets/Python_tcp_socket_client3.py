#!/usr/bin/env python

import socket
from contextlib import closing
import fileinput

port = 12345                 # Reserve a port for your service.
svt_traffic_file = "Rx_From_BTT_REM.txt"

host = socket.gethostname()  # Get local machine name
print 'Connecting to ', host, port

with closing(socket.socket()) as s:
    s.connect((host, port))
    print 'Connected.'

    for line in fileinput.input(svt_traffic_file):
        print "[Sending] %s" % line
        s.send(line)
        # msg = s.recv(1024)
        # print 'SERVER >> ', msg
