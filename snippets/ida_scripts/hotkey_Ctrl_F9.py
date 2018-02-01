import idaapi
import idc

import m68k_util

if __name__ == "__main__":
    print "Ctrl-F9 pressed.."
    cur = 0x2000
    limit = 0x0014E0BE
    print "Start to convert from %X to %X" % (cur, limit)
    (blocks_ok, blocks_notok) = m68k.convert_data_to_bytes(cur, limit)           
    print "Data block converted: %d; Not converted: %d" % (blocks_ok, blocks_notok)