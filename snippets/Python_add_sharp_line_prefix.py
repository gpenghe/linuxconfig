#!/usr/bin/python
#
# 2008-09, gph
# 

import sys

PROGRAM_NAME = "addsharp"

def usage():
    print "Usage: " + PROGRAM_NAME + " [filename]"
    print "Result was written to addsharp.out."
    sys.exit()

if len(sys.argv)!=2:
    usage()

filename=sys.argv[1]
print "Processing file:", filename, "..."
fin=open(filename, "r")
linelist = fin.readlines()
fout=open("addsharp.out", "w")
for line in linelist:
    line = "#"+line
    fout.write(line)

fout.close()
fin.close()
