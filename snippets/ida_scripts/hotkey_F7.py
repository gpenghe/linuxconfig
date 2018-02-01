import idc

import m68k_util
import create_switch


if __name__ == "__main__":
    print "F7 pressed.."
    cur = idc.here()
    create_switch.create_68k_jump_table(cur)