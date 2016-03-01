#!/usr/bin/env python

import sys
sys.dont_write_bytecode = True
import curses
import time

import signal


stdscr = None
win = None


def init_window(x, y, w, h):
    global stdscr
    global win
    stdscr = curses.initscr()
    assert stdscr

    curses.noecho()
    curses.cbreak()
    stdscr.keypad(1)

    win = curses.newwin(h, w, y, x)
    assert win


def uninit_window():
    stdscr.keypad(0)
    curses.nocbreak()
    curses.echo()
    curses.endwin()


def signal_handler(signal, frame):
    print "Exiting..."
    uninit_window()
    sys.exit(0)


def main():
    signal.signal(signal.SIGINT, signal_handler)

    init_window(40, 40, 20, 20)

    i = 1

    stdscr.addstr(1, 1, "Hello stdscr")
    win.addstr(10, 3, "Hello win")
    stdscr.refresh()
    time.sleep(1)

    while True:
        stdscr.clear()
        win.clear()
        stdscr.addstr(0, 0, str(i))
        stdscr.addstr(1, 0, str(i*10))
        win.addstr(10, 0, str(i)*2)
        win.addstr(11, 0, str(i*10*2))
        stdscr.refresh()
        win.refresh()
        time.sleep(1)
        i += 1


if __name__ == "__main__":
    main()
