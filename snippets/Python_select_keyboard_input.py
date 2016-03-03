import sys
import select
import tty


tty.setcbreak(sys.stdin)
while True:
    i, o, e = select.select([sys.stdin], [], [], 0.00001)
    input = sys.stdin.read(1)
    if input == 'q':
        break
    print input
