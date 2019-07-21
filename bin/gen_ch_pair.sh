#!/bin/bash

usage()
{
    cat <<eot
Generate a pair of .c and .h files.

Usage:
    $0 <foo.c>

Example:
    $0 dsc.
This will generate dsc.c and dsc.h

    $0 /tmp/dsc.c
This will generate /tmp/dsc.c and /tmp/dsc.h
eot
}

if [[ $# -lt 1 || $(expr match "$1" "-" &> /dev/null) ]]; then
    usage
    exit 1
fi

dname=$(dirname "$1")
bname=$(basename "$1" .c)
cd $dname
c_filename=$bname.c
h_filename=$bname.h

echo -n "Generating $c_filename and $h_filename"
if [[ "$dname" != "." ]]; then
    echo -n " in directory $dname"
fi
echo ""

def_macro=$(echo _"${h_filename}"_ | tr 'a-z. ' 'A-Z__')
cat > "$h_filename" <<eot
#ifndef $def_macro
#define $def_macro

#endif
eot

cat > "$c_filename" <<eot
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "$h_filename"

//<--------------  Defines    ------------------------------------------

//>---------------------------------------------------------------------

//<--------------  Static Functions    ---------------------------------

//>---------------------------------------------------------------------

//<--------------  Interface Functions ---------------------------------

//>---------------------------------------------------------------------

eot

