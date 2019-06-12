#!/bin/bash

pof=$1

name='Bitstream_0'
start='0x00000000'
len=49020928
echo "Copying $name.bin: start $start, len $len"
_skip=$(python -c "print $start")
let _skip=$_skip+146
dd if=$pof of=$name.bin count=$len bs=1 skip=$_skip

name='Bitstream_1'
start='0x02EC0000'
len=49020928
echo "Copying $name.bin: start $start, len $len"
_skip=$(python -c "print $start")
let _skip=$_skip+146
dd if=$pof of=$name.bin count=$len bs=1 skip=$_skip

name='Image_Data_0'
start='0x05D80000'
len=5636096
echo "Copying $name.bin: start $start, len $len"
_skip=$(python -c "print $start")
let _skip=$_skip+146
dd if=$pof of=$name.bin count=$len bs=1 skip=$_skip

name='Table_0'
start='0x062E0000'
len=131072
echo "Copying $name.bin: start $start, len $len"
_skip=$(python -c "print $start")
let _skip=$_skip+146
dd if=$pof of=$name.bin count=$len bs=1 skip=$_skip

name='MB0_0'
start='0x06300000'
len=2621440
echo "Copying $name.bin: start $start, len $len"
_skip=$(python -c "print $start")
let _skip=$_skip+146
dd if=$pof of=$name.bin count=$len bs=1 skip=$_skip

name='MB1_0'
start='0x06580000'
len=3670016
echo "Copying $name.bin: start $start, len $len"
_skip=$(python -c "print $start")
let _skip=$_skip+146
dd if=$pof of=$name.bin count=$len bs=1 skip=$_skip

name='Image_Data_1'
start='0x06900000'
len=5636096
echo "Copying $name.bin: start $start, len $len"
_skip=$(python -c "print $start")
let _skip=$_skip+146
dd if=$pof of=$name.bin count=$len bs=1 skip=$_skip

name='Table_1'
start='0x06E60000'
len=131072
echo "Copying $name.bin: start $start, len $len"
_skip=$(python -c "print $start")
let _skip=$_skip+146
dd if=$pof of=$name.bin count=$len bs=1 skip=$_skip

name='MB0_1'
start='0x06E80000'
len=2621440
echo "Copying $name.bin: start $start, len $len"
_skip=$(python -c "print $start")
let _skip=$_skip+146
dd if=$pof of=$name.bin count=$len bs=1 skip=$_skip

name='MB1_1'
start='0x07100000'
len=3670016
echo "Copying $name.bin: start $start, len $len"
_skip=$(python -c "print $start")
let _skip=$_skip+146
dd if=$pof of=$name.bin count=$len bs=1 skip=$_skip

name='Image_Data_2'
start='0x07480000'
len=5636096
echo "Copying $name.bin: start $start, len $len"
_skip=$(python -c "print $start")
let _skip=$_skip+146
dd if=$pof of=$name.bin count=$len bs=1 skip=$_skip

name='Table_2'
start='0x079E0000'
len=131072
echo "Copying $name.bin: start $start, len $len"
_skip=$(python -c "print $start")
let _skip=$_skip+146
dd if=$pof of=$name.bin count=$len bs=1 skip=$_skip

name='MB0_2'
start='0x07A00000'
len=2621440
echo "Copying $name.bin: start $start, len $len"
_skip=$(python -c "print $start")
let _skip=$_skip+146
dd if=$pof of=$name.bin count=$len bs=1 skip=$_skip

name='MB1_2'
start='0x07C80000'
len=3670016
echo "Copying $name.bin: start $start, len $len"
_skip=$(python -c "print $start")
let _skip=$_skip+146
dd if=$pof of=$name.bin count=$len bs=1 skip=$_skip
