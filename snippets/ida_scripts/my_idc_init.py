import idaapi
import idc

compiled_functions = {}
def ida_run_python_function(func_name):
    if func_name not in compiled_functions:
        ida_func_name = "py_%s" % func_name
        idaapi.CompileLine('static %s() { RunPythonStatement("%s()"); }' 
            % (ida_func_name, func_name))
        compiled_functions[func_name] = ida_func_name
    return ida_func_name
    
    
#----------------------------------------------------------------------
# Create a switch idiom for m68k
def m68k_create_switch():
    print "creating switch"
    create_68k_jump_table(here())


def hotkey_F4():
    path = 'D:\\Tools\\ida_scripts\\hotkey_F4.py'
    print path
    execfile(path)


def hotkey_F5():
    path = 'D:\\Tools\\ida_scripts\\hotkey_F5.py'
    print path
    execfile(path)
    

def hotkey_F6():
    path = 'D:\\Tools\\ida_scripts\\hotkey_F6.py'
    print path
    execfile(path)
    
      
def hotkey_F7():
    path = 'D:\\Tools\\ida_scripts\\hotkey_F7.py'
    print path
    execfile(path)


def hotkey_F8():
    path = 'D:\\Tools\\ida_scripts\\hotkey_F8.py'
    print path
    execfile(path)

    
def hotkey_Ctrl_F9():
    path = 'D:\\Tools\\ida_scripts\\hotkey_Ctrl_F9.py'
    print path
    execfile(path)

    
def my_init():
    print "Running my_init() in my_idc_init.py"

    AddHotkey("F4", ida_run_python_function("hotkey_F4"))
    AddHotkey("F5", ida_run_python_function("hotkey_F5"))
    AddHotkey("F6", ida_run_python_function("hotkey_F6"))    
    AddHotkey("F7", ida_run_python_function("hotkey_F7"))
    AddHotkey("F8", ida_run_python_function("hotkey_F8"))
    AddHotkey("F9", ida_run_python_function("m68k_create_switch"))
    AddHotkey("Ctrl+F9", ida_run_python_function("hotkey_Ctrl_F9"))

# Running it every time a new file is loaded  
my_init()