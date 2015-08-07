#!/bin/bash

for i in `seq 1 1416`
do
	wget http://xkcd.com/$i/
	url=`grep http://imgs.xkcd.com/comics/ index.html | head -1 | cut -d\" -f2`
    wget $url
    file=`echo $url|cut -d\/ -f5`
    mv $file `printf "%04d" $i`-$file
	rm index.html
done
