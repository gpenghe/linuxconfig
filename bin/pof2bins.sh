#!/bin/bash

names=(
    'Image_Data_0'
    'Table_0'
    'MB0_0'
    'MB1_0'
    'Image_Data_1'
    'Table_1'
    'MB0_1'
    'MB1_1'
    'Image_Data_2'
    'Table_2'
    'MB0_2'
    'MB1_2'
    'Bitstream_0'
    'Bitstream_1'
)
starts=(
    '0x05D80000'
    '0x062E0000'
    '0x06300000'
    '0x06580000'
    '0x06900000'
    '0x06E60000'
    '0x06E80000'
    '0x07100000'
    '0x07480000'
    '0x079E0000'
    '0x07A00000'
    '0x07C80000'
    '0x00000000'
    '0x02EC0000'
)
lengths=(
    5636096
    131072
    2621440
    3670016
    5636096
    131072
    2621440
    3670016
    5636096
    131072
    2621440
    3670016
    49020928
    49020928
)
mod_cnt=${#names[@]}
mod_cnt_starts=${#starts[@]}
mod_cnt_lengths=${#lengths[@]}

usage() {
cat << eot
Usage: $0 <a.pof>

Will output the following binary files that are extracted from respective
addresses with respective lengths. All binaries will be endian-reversed.
For example: a 4-byte data "AA BB CC DD" will becomes "DD CC BB AA".

Extraction Table:
----------------------------------------------------------
  Bin_Name            Pof_Offset            Length
----------------------------------------------------------
eot

while [[ $i -lt $mod_cnt ]]; do
    name=${names[i]}
    _start=${starts[i]}
    len=${lengths[i]}
    printf "%16s     0x%08X  %12d (%08X)\n" $name.bin $_start $len $len
    let i=$i+1
done

cat << eot
----------------------------------------------------------
eot
}

if [[ $# -ne 1 ]]; then
    usage
    exit 1
fi

pof=$1
pof_header_size=146
out_dir=fwbins

if [[ $mod_cnt_starts -ne $mod_cnt ]]; then
    echo "'starts' array length expected: $mod_cnt. Actual $mod_cnt_starts"
    exit 1
fi
if [[ $mod_cnt_lengths -ne $mod_cnt ]]; then
    echo "'lengths' array length expected: $mod_cnt. Actual $mod_cnt_lengths"
    exit 1
fi

i=0;
if [[ ! -d $out_dir ]]; then
    mkdir -v $out_dir
fi
while [[ $i -lt $mod_cnt ]]; do
    name=${names[i]}
    _start=${starts[i]}
    len=${lengths[i]}
    _skip=$(python -c "print $_start")
    let _skip=$_skip+$pof_header_size
    output="$name.bin"
    echo -n "Copying $name.bin: start: $_start, len: $len bytes"
    dd if="$pof" of="$output.reverse" count="$len" bs=1 skip="$_skip" status=none
    endconv.py "$output.reverse"
    mv -v "$output.reverse.out" $out_dir/$output
    rm -v "$output.reverse"
    echo ", crc32: $(md5sum $out_dir/$output |cut -d' ' -f1)"
    let i=$i+1
done
