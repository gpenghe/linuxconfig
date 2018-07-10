# Just run tightvncserver
tightvncserver


# then use `serverip:1` as the vnc server address



#---------------------------------------------
# It works but very slow!

# ~/.vnc/xstartup:

#!/bin/sh

xrdb $HOME/.Xresources
xsetroot -solid grey
# fix to make xfce work:
startxfce4 &

