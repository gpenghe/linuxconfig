#!/bin/bash

usage()
{
    cat <<eot
Generate a pair of .c and .h files.

Usage:
    $0 <foo.c>

Example:
    $0 dsc.
This will generate dsc.c, dsc.h and test_dsc.c

    $0 /tmp/dsc.c
This will generate /tmp/dsc.c, /tmp/dsc.h and /tmp/test_dsc.c
eot
}

if [[ $# -lt 1 || $(expr match "$1" "-" &> /dev/null) ]]; then
    usage
    exit 1
fi

dname=$(dirname "$1")
bname=$(basename "$1" .c)

if [[ ! -d "$dname" ]]; then
    mkdir -v -p $dname
fi
cd "$dname"

c_filename=$bname.c
h_filename=$bname.h
t_filename=test_$bname.c

err=0
for f in "$c_filename" "$h_filename" "$t_filename"; do
    if [[ -f "$f" ]]; then
        echo "Error: $f already exists"
        err=1
    fi
done
[[ $err -eq 1 ]] && exit 1

echo -n "Generating files"
if [[ "$dname" != "." ]]; then
    echo -n " in directory $dname"
fi
echo ":"

##########################################################################
echo -e "\t$h_filename"
def_macro=$(echo _"${h_filename}"_ | tr 'a-z. ' 'A-Z__')
cat > "$h_filename" <<eot
#ifndef $def_macro
#define $def_macro

#endif
eot

##########################################################################
echo -e "\t$c_filename"
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

##########################################################################
echo -e "\t$t_filename"
cat > "$t_filename" <<eot
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>

#include "unity.h"
#include "testdefs.h"

#include "mock_clib_wrapper.h"
#include "mock_usbTypes.h"
#include "mock_usbState.h"
#include "mock_usbDebug.h"
#include "mock_usbLog.h"
#include "mock_usbMessage.h"
#include "mock_usbFifoUtils.h"
#include "mock_string_utils.h"
#include "mock_time_utils.h"
#include "mock_file_utils.h"
#include "mock_int2text.h"
#include "mock_net_utils.h"
#include "mock_sys_utils.h"

#include "calc_utils.h"
#include "msg_utils.h"

#include "mock_var_usbLog.c"
#include "mock_var_osd.c"

#include "$h_filename"

void setUp(void)
{
    msgCommandText_IgnoreAndReturn("foo");
}

void tearDown(void)
{
}

void test_general()
{
    AE(1, 1);
}
eot
