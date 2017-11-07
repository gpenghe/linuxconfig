cat /dev/ttymxc2 |tee out |hexdump -e '/1 "%2X "'

# stty:
#  Optional  -  before  SETTING  indicates negation.  

# 9600, 8-O-1, blocking, min 7 bytes returning read
stty -F /dev/ttymxc2 time 255 min 7 9600 cs8 -cstopb parenb parodd

# 115200, 8-N-1
stty -F /dev/ttymxc2 115200 cs8 -cstopb -parenb
