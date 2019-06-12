#!/usr/bin/env python

import sys
import os

# main
if len(sys.argv) == 1:
    print('Usage: endconv.py <file>')
    sys.exit(0)

myfile = sys.argv[1]
byte_count = 0

ofname = myfile + ".out"
of = open(ofname, "wb");
print "Outputing to %s" % ofname

n = 0
with open(myfile, "rb") as f:
    while True:
        word = f.read(4)
        if len(word) < 4:
            print "Exiting. Read %d bytes" % n
            break
        n += 4
        t = bytearray(4)
        t[0] = word[3]
        t[1] = word[2]
        t[2] = word[1]
        t[3] = word[0]
        of.write(t)
