#!/usr/bin/python
#
# 2014-05, gph
# 


from __future__ import print_function
import sys
import fileinput

# format:
# 0x00000000 cf fa ed fe 07 00 00 01 - 03 00 00 80 02 00 00 00 

# main
outfile = 'out.bin'

if len(sys.argv) == 1:
    print('Usage: genbin.py <file.binhex> [out.bin]')
    sys.exit(0)
elif len(sys.argv) >= 3:
    outfile = sys.argv[2]

myfile = sys.argv[1]
clean_copy = myfile + '.clean'

startaddr_checked = False
short_line_encountered = False

with open(outfile, 'wb') as of:
    with open(clean_copy, 'wb') as cf:
        for line in fileinput.input(myfile):
            if not line.startswith('0x0'):
                continue
            assert not short_line_encountered, 'Non-16 bytes line encountered in the middle of the file'
            cf.write(line)
            line = line.replace('- ', '')
            line = line.replace(':', '')
            line = line.strip()
            # (line:) 0x00000000 cf fa ed fe 07 00 00 01 - 03 00 00 80 02 00 00 00 
            # (line:) 0x00000000 cf fa ed fe 07 00 00 01 03 00 00 80 02 00 00 00 
            items = line.split(' ')
            # (items:) 0x00000000 cf fa ed fe 07 00 00 01 03 00 00 80 02 00 00 00 
            addr = items[0]  # (addr:) 0x00000000
            addr = int(addr, 16);
            if not startaddr_checked:
                assert addr == 0, 'The first address should be 0 (instead of 0x%x)' % addr
                startaddr_checked = True
            items = items[1:];
            # (items:) cf fa ed fe 07 00 00 01 03 00 00 80 02 00 00 00 
            byte_count = len(items)
            if byte_count != 16:
                assert byte_count <16, 'Only support 16 bytes per line (instead of %d):\n %s' % (byte_count, line)
                short_line_encountered = True
            for i in xrange(0, byte_count):
                items[i] = int(items[i], 16)
            # (items:) (all dec)
            ba = bytearray(items)
            of.write(ba)
