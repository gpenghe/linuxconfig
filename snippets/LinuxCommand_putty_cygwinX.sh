putty setup:

X display localtion: ":0.0"



CygwinX:
change "XWin Server" shortcut to:
C:\cygwin64\bin\run.exe --quote /usr/bin/bash.exe -l -c "cd; /usr/bin/xinit /etc/X11/xinit/startxwinrc -- /usr/bin/XWin :0 -multiwindow -listen tcp"

