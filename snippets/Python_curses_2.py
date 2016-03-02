#!/usr/bin/env python

import sys
sys.dont_write_bytecode = True
import curses
import time

import signal


win = None
g_screen_updated = True
g_screen_string = "Example Line 1\n\nExample Line 2"


def signal_handler(signal, frame):
    print "Exiting..."
    sys.exit(0)


def main(stdscr):
    global g_screen_updated
    signal.signal(signal.SIGINT, signal_handler)

    # nlines=2, ncols=40, begin_y=20, begin_x=20
    win = curses.newwin(2, 40, 20, 20)
    assert win
    win.border()

    stdscr.clear()
    stdscr.addstr(10, 40, "Otis Service Tool Emulator")
    stdscr.refresh()

    while True:
        win.clear()
        if g_screen_updated:   # XXX: fix it with mutex
            lines = g_screen_string.split("\n")
            oneline_printed = False
            twolines_printed = False
            for line in lines:
                if line:
                    if not oneline_printed:
                        win.addstr(0, 0, line)
                        oneline_printed = True
                    elif not twolines_printed:
                        win.addstr(1, 0, line)
                        twolines_printed = True
                    else:
                        # We already print two lines.
                        # Ignore the remaining lines
                        break
            win.refresh()
            g_screen_updated = False
        else:
            time.sleep(0.001)


if __name__ == "__main__":
    curses.wrapper(main)
