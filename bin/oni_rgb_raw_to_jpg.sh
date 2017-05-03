# input: foo.raw
# output: foo.jpg

outfile=`basename $1 .raw`.jpg
echo Converting $1 to $outfile
convert -size 640x480 -depth 8 rgb:$1 $outfile
