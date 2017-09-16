# input: foo.png
# output: foo_8bit.png

outfile=`basename $1 .png`_8bit.png
echo Converting $1 to 8 bit $outfile
convert $1 -evaluate multiply 8 -depth 8 $outfile
