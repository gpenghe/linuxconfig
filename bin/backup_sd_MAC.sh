#!/bin/bash

# NEED sudo???

SD=/dev/disk1s1
RAW_SD=/dev/rdisk1

if [ $# -ne 1 ]; then
    echo "Usage: $0 name_postfix"
    exit 0
fi
postfix=$1
imgname=Pi-`date +%Y-%m-%d-$postfix`.img 

if ! [ -e $SD ]; then
    echo "Make sure the SD is inserted" >/dev/stderr
    exit -1
fi

if [ -f $imgname ]; then
    echo "$imgname already exists!" >/dev/stderr
    exit -1
fi

echo "Umounting $SD..."
diskutil umount $SD 

echo "Copying SD to $imgname... "
#dd if=$RAW_SD of=$imgname bs=1m && "echo gzipping the image file..." && gzip $imgname && echo "Success!" || echo "Failed!"
dd if=$RAW_SD of=$imgname bs=1m && echo "Success!" || echo "Failed!"
