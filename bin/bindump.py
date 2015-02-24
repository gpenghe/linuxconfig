#!/usr/bin/python

from __future__ import print_function
import sys
import os

# configuration
width = 16

halfw = width /2

# main
if len(sys.argv) == 1:
    print('Usage: bindump.py <file>')
    sys.exit(0)

myfile = sys.argv[1]
byte_count = 0

with open(myfile, "rb") as f:
    byte = f.read(1)
    while byte != "":
        if byte_count % width == 0:
            print("0x%08x " % byte_count, end='')
        print('%02x ' % ord(byte), end='')
        byte = f.read(1)
        byte_count += 1
        if byte_count % width == 0:
            print('\n', end='')
        elif byte_count % halfw == 0:
            print('- ', end='')

print('')
