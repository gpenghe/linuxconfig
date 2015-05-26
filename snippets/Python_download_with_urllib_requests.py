#!/usr/bin/env python

import urllib
import sys

sys.path.append('/home/subhash/subh/python/requests')
#adding the requests http://docs.python-requests.org/en/latest/index.html
import requests
print sys.argv[1]
# url to download
r=requests.get(sys.argv[1])
with open(sys.argv[2],"wb") as code:
#argv[2] is target file name
 code.write(r.content)
