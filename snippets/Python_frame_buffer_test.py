#!/usr/bin/python
#
# 2007-11, gph
#

count=640*480;
#red='\xf8\x00';
red='\xff';

f=open("/dev/fb0","w");
for i in range(0,count):
    f.write(red);
f.close();
