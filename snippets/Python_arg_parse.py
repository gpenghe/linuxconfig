#!/usr/bin/env python

import sys
import os
import fileinput

###########################
#   Configurations
###########################

g_debug     = False
arg_verbose = False
arg_sym_file = "busybox_unstripped"

###########################
#   Global Variables
###########################

g_name_mapping = {}   # dict of address->name 
###########################
#   Functions
###########################
def print_cfg_info():
    namelist = sorted(globals().keys())
    for name in namelist:
        if name.startswith('arg_'):
            print name, ":\t", eval(name)
    print ''

def gen_name_mapping(sym_file):
    for line in fileinput.input(sym_file):
        fields = line.split()
        if fields and fields[0].startswith('000'):
            if (len(fields) == 5) or (len(fields) == 6):
# len 5 example:
#       0000000000407640       F *UND*  0000000000000000              sprintf@@GLIBC_2.2.5          # len 6 example:
#       0000000000400238 l    d  .interp        0000000000000000              .interp
                address = hex(int("0x" + fields[0], 16))
                address = address[2:]
                func = fields[-1]
                g_name_mapping[address] = func

def process(log_file):
    print "Reformatting %s:" % log_file
    for line in fileinput.input(log_file):
        fields = line.split()
        #print fields[0] , g_name_mapping[fields[1]], ";" + fields[2] + ";" + fields[3]
        try:
            func1 = g_name_mapping[fields[1]]
        except Exception:
            func1 = "UNKNOWN"
        try:
            func2 = g_name_mapping[fields[2]]
        except Exception:
            func2 = "UNKNOWN"
        print fields[0]                 + "," + \
            func1                       + "," + \
            func2                       + "," + \
            fields[3]                   + "," + \
            fields[4]

###########################

if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser(
            description='Control Flow Graph generation from a instrumentation log',
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog='''
Examples:
    $ ./gen_graph.py --sym-file objdump.log stack_trace-2015-04-14_15_19_10.log > test.csv
                    '''
            )
    parser.add_argument('-p', '--print-value', help='Print configuration values', action='store_true', default=False)
    parser.add_argument('-v','--verbose', help='Use verbose output', action='store_true', default=arg_verbose)
    parser.add_argument('--sym-file', help='The symbol file of unstripped elf binary file', default=arg_sym_file);
    parser.add_argument('file', help='name of the instrumentation log file')

    args = parser.parse_args()

    arg_verbose     = args.verbose

    g_log_file = args.file
    arg_sym_file = args.sym_file

    if args.print_value == True:
        print_cfg_info()
        sys.exit(0)

    gen_name_mapping(arg_sym_file)
    #for k,v in g_name_mapping.iteritems():
    #    print k, v

    process(g_log_file)
