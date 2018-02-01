import idc

def makedw():
    ea = idc.SelStart()
    if ea == idc.BADADDR:  # No selection
        ea = idc.ScreenEA()
        if idc.Byte(ea) == 0:
            print "Avoid working right after the previous string list"
            return
        ea = ea & 0xFFFFFFFC
        end = ea
    else:
        print "Selected 0x%08X, 0x%08X" % (idc.SelStart(), idc.SelEnd())
        ea  = (idc.SelStart() & 0xFFFFFFFC) +4
        end = idc.SelEnd() & 0xFFFFFFFC
    print "[0x%08X - 0x%08X]" % (ea, end)
    while ea <= end:
        if (idc.Byte(ea) == 1) and (idc.Byte(ea+1) == 7): 
            print "Found 1, 7"
            print "We might have encountered the string list"
            break
        idc.MakeUnkn(ea, idc.DOUNK_SIMPLE)
        idc.MakeDword(ea)
        ea += 4
    
    #idc.Jump(ea)
   
def makeunk():
    ea = idc.ScreenEA()
    idc.MakeUnkn(ea, idc.DOUNK_SIMPLE)
    
def delfunc():
    rt = idc.DelFunction(idc.ScreenEA())
    if rt == 0:
        print "Error deleting function"
    
# Hotkeys
# Currently available:
#    F4, F5, F6, F7, F8
def hotkey_F4():
    print "hotkey_F4"
    makedw()
  
def hotkey_F5():
    print "hotkey_F5"
    makeunk()
  
def hotkey_F6():
    print "hotkey_F6 pressed"
    delfunc()
    
def hotkey_F8():
    print "hotkey_F8"
    ea = idc.ScreenEA()
    idc.MakeUnkn(ea, idc.DOUNK_SIMPLE)
    idc.MakeFloat(ea)   
    idc.Jump(ea+4)