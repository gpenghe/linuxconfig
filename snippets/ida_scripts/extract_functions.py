with open('func.list', 'w') as outfile:
  funcs = Functions()
  for f in funcs:
    name = Name(f)
    if not name.startswith('sub'):
      start = GetFunctionAttr(f, FUNCATTR_START)
      end = GetFunctionAttr(f, FUNCATTR_END)
      s = "%s,%08X,%08X\n" % (name, start, end)
      outfile.write(s)