#!/bin/sh
# 

if [ $# -ne 2 ]; then
    echo "Usage:"
    echo "    $0 <video_file> <avi_file>"
    exit 0
fi

if [ -e $2 ]; then
    echo "$2 already exists. Overwrite it? (Y/N)"
    read -d'' -s -n1
    key=`echo $REPLY | tr [a-z] [A-Z]`
    echo
    if [ $key != 'Y' ]; then
        echo "Not overwriting..."
        exit 0
    fi

    # OK to delete it
    rm -f $2
fi

bitrate="4000K"
echo Encoding $1 to $2 with $bitrate bitrate ...
ffmpeg -i $1 -b:v $bitrate $2 &> /dev/null

# A more complex method. Not needed
# ffmpeg -r 30 -i $1 -c:v mpeg4 -flags:v +qscale -global_quality:v 0 -codec:a libmp3lame $2
