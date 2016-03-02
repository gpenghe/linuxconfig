#!/usr/bin/env python

import sys
sys.dont_write_bytecode = True
import curses

import signal


def signal_handler(signal, frame):
    print "Exiting..."
    sys.exit(0)


def main(stdscr):
    signal.signal(signal.SIGINT, signal_handler)

    stdscr.clear()

    curses.init_pair(1, curses.COLOR_BLACK, curses.COLOR_RED)
    curses.init_pair(2, curses.COLOR_BLACK, curses.COLOR_GREEN)

    stdscr.attron(curses.color_pair(1))
    stdscr.addstr("black on red\n\n")

    stdscr.attron(curses.color_pair(2))
    stdscr.addstr("black on green\n\n")

    stdscr.attroff(curses.color_pair(2))
    stdscr.addstr("normal text\n\n")

    stdscr.refresh()

    stdscr.getch()

if __name__ == "__main__":
    curses.wrapper(main)
