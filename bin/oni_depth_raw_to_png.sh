# input: foo.raw
# output: foo.png

outfile=`basename $1 .raw`.png
echo Converting $1 to $outfile
convert -size 640x480 -depth 16 gray:$1 $outfile
