import struct
from socket import *

serverSocket = socket(AF_INET, SOCK_DGRAM)
serverSocket.bind(('', 50020))
while True:
    message, address = serverSocket.recvfrom(1024)
    (n1, _) = struct.unpack("II", message)
    print "Received UDP message #%d at port 50020 (len %d) from %s" % (
        n1, len(message), str(address))
