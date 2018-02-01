import idaapi
import idc

def search_non_code(start, limit):
    """
    Jumping to next non-function or data.

    Returns:
    (search_ok, result_ea)
    """
    
    print "Running m68k_search_non_code!!"
    
    search_ok = False
    result_ea = -1
    
    cur = start
    while True:
        if cur >= limit:
            break

        u = idc.FindUnexplored(cur, idc.SEARCH_DOWN)
        d = idc.FindData(cur, idc.SEARCH_DOWN)
        n = idaapi.find_not_func(cur, idc.SEARCH_DOWN)
        cur = min(u, d, n)     
        
        cur_byte = idc.GetManyBytes(cur, 1, False)
        if not cur_byte:
            print "Failed to get byte"
            break
        cur_byte = ord(cur_byte)
        prev = idc.PrevHead(cur, 0)
        # Skip the following:
        #   - prev is a switch table
        #   - prev is a 'rts'
        #   - cur is FF
        if idc.GetMnem(prev) == "jmp":
            # print "Skipping jmp"
            continue
        elif idc.GetMnem(prev) == "rts":
            # print "Skipping rts"
            continue      
        elif cur_byte == 0xff:
            continue
        else:
            search_ok = True
            result_ea = cur
            break

    return (search_ok, result_ea)


def make_code(start, limit):
    blocks_ok = 0
    blocks_notok = 0
    cur = start
    while True:
        if cur >= limit:
            print "Stopping at limit %08X\n" % limit
            break
        u = idc.FindUnexplored(cur, SEARCH_DOWN)
        d = idc.FindData(u-2, SEARCH_DOWN)    # workaround
        cur = min(u, d)
        print "cur: %X" % cur

        # Skip FF
        cur_byte = idc.GetManyBytes(cur, 1, False)
        if not cur_byte:
            print "Failed to get byte"
            break
        cur_byte = ord(cur_byte)
        # print "Current byte: %08X" % cur_byte
        is_ff = (cur_byte == 0xff)
        if is_ff:
            cur +=1  # because sometimes we are backward for 1 byte
            continue
        else:
            rt = idc.MakeCode(cur)   
            if rt == 0:
                blocks_notok += 1
            else:
                # Jump(cur)            
                blocks_ok +=1
                if blocks_ok % 10 == 0:
                    print "Converted blocks: %d" % blocks_ok
        cur += 2        # because sometimes we are backward for 1 byte
        # break  # for debug
    return (blocks_ok, blocks_notok)
    

def make_function(ea):
    """
    Convert data to a function
    """
    rt = MakeUnkn(ea, DOUNK_EXPAND)
    if not rt:
        print "MakeUnkn() failed"
    rt = idc.MakeCode(ea)
    if not rt:
        print "MakeCode() failed"
    rt = idc.MakeFunction(ea, BADADDR)
    if not rt:
        print "MakeFunction() failed"

    
def convert_data_to_bytes(start, limit):
    """
    To make it easier to convert data to code
    """
    
    cur = start
    rt = idc.MakeUnkn(cur, idc.DOUNK_EXPAND)
    if not rt:
        print "MakeUnkn() failed"
    rt = idc.MakeCode(cur)
    if not rt:
        print "MakeCode() failed"    
    rt = idc.MakeFunction(cur, idc.BADADDR)
    if not rt:
        print "MakeFunction() failed"    
        
    blocks_ok = 0
    blocks_notok = 0
    while True:
        if cur >= limit:
            print "Stopping at limit %08X\n" % limit
            break
        cur = FindData(cur, SEARCH_DOWN)
        print "Found %X" % cur
        
        # skip switch table
        prev = PrevHead(cur, 0)
        if GetMnem(prev) == "jmp":
            continue
            
        Jump(cur)              
        rt = MakeByte(cur)   
        if rt == 0:
            blocks_notok += 1
        else:     
            blocks_ok +=1
            if blocks_ok % 10 == 0:
                print "Converted blocks: %d" % blocks_ok        
                
    return (blocks_ok, blocks_notok)