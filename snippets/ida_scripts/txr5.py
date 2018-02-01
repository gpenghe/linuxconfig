import idaapi 
import idc

import m68k_util

def hotkey_F4():
    cur = 0x500
    limit = 0x45d2
    print "Start to make code from %X to %X" % (cur, limit)
    (blocks_ok, blocks_notok) = m68k_util.make_code(cur, limit)
    print "Block converted: %d; Not converted: %d" % (blocks_ok, blocks_notok)