#!/usr/bin/env python

import urllib

n = 12345
s = str(n)
i = 1

while i <= 400:
    s = urllib.urlopen("http://www.pythonchallenge.com/pc/def/linkedlist.php?nothing=" + s).readlines()[-1].rstrip().split(' ')[-1]
    n = int(s)
    if s == '16044':
        n /= 2
    print "(%3d) %8d" % (i, n)
    s = str(n)
    i += 1
