#!/bin/bash -e

###################################################################
# Output IP/Broadcast for other program's consumption.
###################################################################

function usage() {
    echo "output IP/broadcast for other program consumption"
    echo ""
    echo "usage: $0 <-h|ip|bcast>"
}

get_ip=0
get_bcast=0

if [[ $# -ne 1 ]] || [[ $1 == "-h" ]] ; then
    usage
    exit 1
fi

# All possible interfaces, sorted by priority
if [[ $1 == "ip" ]] ; then
    get_ip=1
elif [[ $1 == "bcast" ]] ; then
    get_bcast=1
fi

# we will just output a simple string with no fancy output allowed
# shellcheck disable=SC1001,SC2010
{
    wlan_interfaces=$(\ls /sys/class/net|\grep '^[w]' ||:)
#    eth_interfaces=$(\ls /sys/class/net|\grep '^[e]' ||:)
}
for i in $wlan_interfaces $eth_interfaces; do
# shellcheck disable=SC2034
{
    if ( ip addr show $i | grep "inet " - ) &> /dev/null; then
         # shellcheck disable=SC2016
        if [[ $get_ip -eq 1 ]]; then
            cmd="ip addr show $i | grep 'inet ' | head -n 1 | sed 's#.*inet \(.*\)/.*#\1#'"
            eval $cmd
            exit 0
        fi
        if [[ $get_bcast -eq 1 ]]; then
            cmd="ip addr show $i | grep 'inet ' | head -n 1 | sed 's#.*brd \(.*\) scope.*#\1#'"
            eval $cmd
            exit 0
        fi
    fi
}
done

exit 1
