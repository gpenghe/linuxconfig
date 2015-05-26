#!/usr/bin/env python

import cv2
import numpy as np

#
# Camera / VideoCapture
#
#cap = cv2.VideoCapture('reed.mov') 
#cap = cv2.VideoCapture(0) # works for Macbook Air internal camera
cap = cv2.VideoCapture(1) # works for Microsoft camera in Macbook Air clamshell mode
ret, frame = cap.read()
w2, h2 = frame.shape[1]/2, frame.shape[0]/2 # new size (w,h)

#
#  VideoWriter
#
fps = 15
fourcc = cv2.cv.CV_FOURCC('m','p','4','v')
out = cv2.VideoWriter('out.mov', fourcc, fps, (w2,h2) )

#
#  Loop over each frame
#
while(True):
    ret, frame = cap.read();
    sm = cv2.resize(frame, (w2,h2))
    cv2.imshow('small', sm)
    out.write(sm) # can only write color video
    gray = cv2.cvtColor(sm, cv2.COLOR_BGR2GRAY)
    cv2.imshow('gray', gray)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

#
#  Clean up
#
cap.release()
out.release()
cv2.destroyAllWindows()


