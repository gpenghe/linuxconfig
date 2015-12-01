import signal
import sys
import time

def signal_handler(signal, frame):
    print "Exiting..."
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

while True:
    time.sleep(1)
