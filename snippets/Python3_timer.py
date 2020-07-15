import time, threading

WAIT_SECONDS = 1

def foo():
    print(time.ctime())
    threading.Timer(WAIT_SECONDS, foo).start()
    
foo()

#output:
#Thu Dec 22 14:46:08 2011
#Thu Dec 22 14:46:18 2011
#Thu Dec 22 14:46:28 2011
#Thu Dec 22 14:46:38 2011
