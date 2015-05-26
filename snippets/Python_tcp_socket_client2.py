import sys
import time
import socket

#HOST = sys.argv[1];
HOST = '192.168.0.1'
PORT = 50007              # The same port as used by the server
print 'Connecting to lock at %s:%s' % (HOST, PORT)

print 'Request to open lock...'
starttime = time.time()

while True:
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((HOST, PORT))
    except socket.error as msg:
        time.sleep(0.1) # wait then loop trying to connect
        continue
    break

s.sendall('Request to open Lock')
data = s.recv(1024)
duration = time.time() - starttime
s.close()

print '\nLock replied %s in %.5f seconds: ' % (repr(data), duration)


