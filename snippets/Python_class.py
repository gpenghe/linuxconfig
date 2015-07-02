#!/usr/bin/env python

import re
from inspect import currentframe, getframeinfo
import datetime

###########################
#   DEFINES
###########################

STATE_UNKNOWN,                  \
STATE_SEARCHING_HEADER,         \
STATE_START_READING_DATA,       \
STATE_READING_DATA,             \
STATE_DATA_DONE,                \
= range(5)

SESSION_STATE_UNKNOWN,                      \
SESSION_STATE_SEARCHING_SESSION_HEADER,     \
SESSION_STATE_SESSION_START,                \
SESSION_STATE_SESSION_IN_PROGRESS,          \
SESSION_STATE_SESSION_ENDED,                \
= range(5)

DIR_INVALID, \
DIR_TX, \
DIR_RX, \
DIR_BOTH \
= range(4)


###########################
#   Configurations
###########################

g_debug = False

arg_verbose = False
arg_line_number_limit = 0
arg_from_session = 0
arg_to_session = 99999
arg_direction = DIR_BOTH

###########################
#   Class Definitions
###########################

class DataItem:
    """ Represent a single NMS data packet """
    tx = True
    func_code = 0
    size = 0
    timestamp = None

    t_diff = 0
    t_duration = 0
    start_second = 0.0

    @classmethod
    def header_string(cls):
        s = "T/R  FuncCode   Size(bytes)        TimeStamp        tStart(s)   tDiff(s)   tDuration(s)  Rate(Bps) \n"
        s+= "======================================================================================================"
        return s

    def __repr__(self):
        tx_flag = "T" if self.tx else "R"
        rate = 'Unknown' if self.t_duration == 0 else ( "%5d" % int(self.size/self.t_duration) ) 
        return "%s        %s     %4d   %s   %7d.%03d  %5d.%03d   %5d.%03d       %s" % (
                tx_flag, 
                self.func_code, 
                self.size, 
                str(self.timestamp),
                int(self.start_second), int(self.start_second*1000%1000),
                int(self.t_diff), int(self.t_diff*1000%1000),
                int(self.t_duration), int(self.t_duration*1000%1000),
                rate
                )
    def init1(self, tx, func_code, size, timestamp):
        self.tx = tx
        self.func_code = func_code
        self.size = size
        self.timestamp = timestamp
    def __init__(self, tx, func_code, size, NMS_date_string):
        # format: Aug 17 11:32:01.926
        year = 2014
        month = 8
        tmp1 = NMS_date_string.split()
        day = int(tmp1[1])       # 17
        tmp2 = tmp1[2].split('.')
        us = int(tmp2[1]) * 1000        # 926000 for us
        tmp3 = tmp2[0].split(':')
        hour    = int(tmp3[0])             # 11
        minute  = int(tmp3[1])             # 32
        second  = int(tmp3[2])             # 1
        dt = datetime.datetime(year, month, day, hour, minute, second, us)
        self.init1(tx, func_code, size, dt)

###########################
#   Functions
###########################

def print_cfg_info():
    namelist = sorted(globals().keys())
    for name in namelist:
        if name.startswith('arg_'):
           print "%s: %d" % (name, eval(name))
    print ''

def de_slip(s):
    """ 
    De-Slip a DBC message string

    Process:
        1. Strip off the "C0" in the beginning of the packet.
        2. Scan for all occurrences of 2 bytes "DB", "DC", and replace them with "C".
        3. Scan for all occurrences of 2 bytes "DB", "DD", and replace them with "DB".
        4. Strip off the "C0" at the end of the packet.

    Args:
        s: a NMS message string like 'C0 AA BB CC C0']

    Example:
    >>> s = "C0 23 DB DC CC 11 DB DD C0"
    >>> s2 = de_slip(s)         # s2 should be "23 C0 CC 11 DB"
    """
    bytelist  = s.split()
    bytelist = bytelist[1:-1]   # step 1 & 4
    out = list()
    last_byte = ""
    for i, byte in enumerate(bytelist):
        if byte == "DB":
            pass
        elif i == 0:
            out.append(byte)
        elif last_byte == "DB":
            if byte == "DC":
                out.append("C0")
            elif byte == "DD":
                out.append("DB")
            else:
                out.append("DB")
                out.append(byte)
        else:
            out.append(byte)
        last_byte = byte
    return ' '.join(out)

def main(nms_data_file):
    state = STATE_SEARCHING_HEADER
    tx = True
    data_item_lines = 0
    lineno = 0
    code_lineno = 0
    in_file = open(nms_data_file, 'r')
    process_same_line = False
    read_complete = False
    date_string = ''
    g_start_time = None

    session = []
    session_no = 0
    session_state = SESSION_STATE_SEARCHING_SESSION_HEADER

    # Passing the whole file line by line
    if arg_verbose:
        print DataItem.header_string()

    while True:
        if not process_same_line:
            l = in_file.readline()
            lineno += 1
        if not l:
            read_complete = True
            break
        else:
            if (arg_line_number_limit and (lineno > arg_line_number_limit)) or \
              session_no > arg_to_session:
                read_complete = True
                break
        process_same_line = False
        if state == STATE_SEARCHING_HEADER:
            # search header
            if re.match('^Transmitted by NMS', l) or re.match('^Received    by NMS', l):
                if re.match('^Transmitted by NMS', l):
                    tx = True
                elif re.match('^Received    by NMS', l):
                    tx = False
                state = STATE_START_READING_DATA
                date_string = l[-19:]        # Aug 17 11:29:12.525
        elif state == STATE_START_READING_DATA:
            if re.match('^C0 ', l):
                state = STATE_READING_DATA
                data_string = l.rstrip()
                data_item_lines = 1
            else:
                code_lineno = getframeinfo(currentframe()).lineno
                break
        elif state == STATE_READING_DATA:
            if re.match('^[\dABCDEF]', l):  # it's a data line
                data_string += ' ' + l.rstrip()
                data_item_lines += 1
            else:
                # we've got the full data item in the previous line
                state = STATE_DATA_DONE
                process_same_line = True
                data = data_string.split()
                length = len(data)
                de_slipped_string = de_slip(data_string)
                de_slipped_bytes = de_slipped_string.split()
                func_code = de_slipped_bytes[14]

                di = DataItem(tx, func_code, length, date_string)
                # Session Processing Start

                if func_code == '40': # Mark the beginning of a session AND end of the previous session
                    if session_state == SESSION_STATE_SEARCHING_SESSION_HEADER:
                        session_state = SESSION_STATE_SESSION_START
                    # fall through
                    if session_state == SESSION_STATE_SESSION_IN_PROGRESS:
                        # we've got a complete session
                        session_state = SESSION_STATE_SESSION_ENDED
                        # print out the last session
                        if session_no >= arg_from_session and session_no <= arg_to_session:
                            if arg_verbose and session_no >= arg_from_session and session_no <= arg_to_session:
                                print '------------- %d ------------------' % session_no

                            # Loop through each DBC data item in a session
                            for i, item in enumerate(session):
                                if i==0:    # first '40' packet
                                    item.t_diff = 0
                                    last_t = item.timestamp
                                    if g_start_time is None:
                                        g_start_time = item.timestamp
                                        item.start_second = 0
                                    else:
                                        item.start_second = (item.timestamp - g_start_time).total_seconds()
                                else:
                                    item.start_second = (item.timestamp - g_start_time).total_seconds()
                                    item.t_diff = (item.timestamp - last_t).total_seconds()
                                    last_t = item.timestamp

                                    # t_duration calculation method:
                                    # Assuming only these patterns possible:
                                    #   T (t) R:    
                                    #       Both T and R would assume to use t/3 time
                                    #   T (t) R (t2) R:    
                                    #       The second R would assume to use t2/2 time
                                    #   T (end of session):
                                    #       Assume T use 0.1 s of time
                                    if item.tx: # T
                                        assert(not session[i-1].tx)
                                        if i == (len(session) -1): # last T
                                            item.t_duration = 0.1
                                        else:
                                            # t_duration would be calculated in next R item
                                            pass
                                    else: # R
                                        if session[i-1].tx: # T
                                            item.t_duration = item.t_diff /3
                                            session[i-1].t_duration = item.t_diff /3
                                        else: # R
                                            item.t_duration = item.t_diff /2

                            for item in session:
                                if ( item.tx and (arg_direction & DIR_TX) ) or \
                                        ( (not item.tx) and (arg_direction & DIR_RX) ):
                                    print_msg = str(item)
                                    if g_debug:
                                        print_msg += "   %s ... %s " % (" ".join(data[:16]), " ".join(data[-2:]))
                                    print print_msg

                            if arg_verbose:
                                item_count = len(session)
                                session_duration = (session[len(session)-1].timestamp - session[0].timestamp).total_seconds()
                                session_footer = '-------------- (#:%d Session Time: %.3f) ----------------\n' % (item_count, session_duration) if arg_verbose else ''
                                print session_footer,

                        # start a new session
                        session_state = SESSION_STATE_SESSION_START
                        session_no += 1
                    # fall through
                    if session_state == SESSION_STATE_SESSION_START:
                        session = []
                        session.append(di)
                        session_state = SESSION_STATE_SESSION_IN_PROGRESS
                elif session_state != SESSION_STATE_SEARCHING_SESSION_HEADER:
                    session.append(di)  # All other data items other than 40 are added to the session object directly
                else: 
                    pass        # keep searching session header

                # Session Processing End
                if g_debug and session_no >= arg_from_session and session_no <= arg_to_session:
                    print "(%s)" % str(di)

        elif state == STATE_DATA_DONE:
            if re.match('^NMS CRC', l):
                # read CRC
                pass
            elif re.match('^\s*$', l): # empty line
                state = STATE_SEARCHING_HEADER
            else:
                code_lineno = getframeinfo(currentframe()).lineno
                break
        else:
            print "Unknown state: %d" % state
            sys.exit(-1)

    # error processing
    if not read_complete:
        print "(%d) Error at state %d (session state: %d) and input line %d: %s" % (code_lineno, state, session_state, lineno, l)
        sys.exit(-1)

###########################
#   Entry point
###########################
if __name__ == "__main__":
    import sys
    import argparse

    parser = argparse.ArgumentParser(description='Transform the raw NMS data file')
    parser.add_argument('-p', '--print-value', help='Print configuration values', action='store_true', default=False)

    parser.add_argument('-v','--verbose', help='Use verbose output', action='store_true', default=arg_verbose)
    parser.add_argument('-n','--number', help='Total number of lines to process (default: 0 for until end of file)', default=arg_line_number_limit, type=int)
    parser.add_argument('-s','--from_session', help='Only output result from this session number (default: from the first session)', default=arg_from_session, type=int)
    parser.add_argument('-t','--to_session', help='Output result until this session number (default: output all sessions)', default=arg_to_session, type=int)
    parser.add_argument('-d','--direction', help='Which direction will be included in the result (one of these: 1 for TX, 2 for RX and 3 for both)', default=arg_direction, type=int)
    parser.add_argument('nms_data_file', nargs='?', help='NMS data input file to process')

    args = parser.parse_args()

    arg_verbose                 = args.verbose
    arg_line_number_limit       = args.number
    arg_from_session            = args.from_session
    arg_to_session              = args.to_session
    arg_direction               = args.direction

    if args.print_value == True:
        print_cfg_info()
        sys.exit(0)

    if args.nms_data_file is None:
        parser.print_help()
        sys.exit(1)

    main(args.nms_data_file)
