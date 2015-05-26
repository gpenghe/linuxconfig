#!/usr/bin/env python

import cv2

#cap = cv2.VideoCapture('Video_001.mp4')
cap = cv2.VideoCapture(1)

fgbg = cv2.BackgroundSubtractorMOG()
#fgbg = cv2.BackgroundSubtractorMOG2()

ret, frame = cap.read()
w2, h2 = frame.shape[1]/4, frame.shape[0]/4 # new size (w,h)

while(True):
    ret, frame0 = cap.read()
    frame = cv2.resize(frame0, (w2,h2))

    cv2.imshow('frame', frame)
    fgmask = fgbg.apply(frame)
    cv2.imshow('foreground', fgmask)
    k = cv2.waitKey(30) & 0xff
    if k == 27: # ESC
        break

cap.release()
cv2.destroyAllWindows()
        
