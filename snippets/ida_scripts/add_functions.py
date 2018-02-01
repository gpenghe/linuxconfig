function_list_dir = os.path.dirname(__file__)
function_list_file = 'func.list'
path = os.path.join(function_list_dir, function_list_file)

cnt_before = len(list(Functions()))
added = 0
with open(path, 'r') as infile:
    while True:
        line = infile.readline().rstrip()
        if line == '':
            break
        t = line.split(',')
        name = t[0]
        start = int(t[1], 16)
        end = int(t[2], 16)
        # Create the imported function in the following cases
        #   * No existing function defined at this location
        #   * There is an existing function but it's named as "sub_XXXX"
        if Name(start) == '' or Name(start).startswith('sub_'):  
            if Name(start).startswith('sub_'): # remove it first
                DelFunction(start)
            rt = MakeFunction(start, end)
            if rt:
                added += 1
                MakeNameEx(start, name, SN_PUBLIC | SN_NON_AUTO | SN_NOWARN)
                Message("%s added\n" % name)
            else:
                Message("Failed to add %s (%08X)\n" % (name, start))


cnt_after = len(list(Functions()))
        
Message("%d new functions added (before: %d, after:%d)\n" % (added, cnt_before, cnt_after))
            