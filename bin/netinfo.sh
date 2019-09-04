#!/bin/bash

###################################################################
# Output IP/Netmask/Broadcast for other program's consumption.
###################################################################

get_ip=0
get_bcast=0
get_mask=0

if [[ $# -ne 1 ]] || [[ $1 == "-h" ]] ; then
    echo "output IP/netmask for other program consumption"
    echo ""
    echo "usage: $0 <-h|ip|bcast|mask>"
    exit 1
fi

# All possible interfaces, sorted by priority
if [[ $1 == "ip" ]] ; then
    get_ip=1
elif [[ $1 == "mask" ]] ; then
    get_mask=1
elif [[ $1 == "bcast" ]] ; then
    get_bcast=1
fi

# we will just output a simple string with no fancy output allowed
# shellcheck disable=SC1001,SC2010
{
    wlan_interfaces=$(\ls /sys/class/net|\grep '^[w]' ||:)
#    eth_interfaces=$(\ls /sys/class/net|\grep '^[e]' ||:)   # not using ethernet interface for Discovery
}
for i in $wlan_interfaces $eth_interfaces; do
# shellcheck disable=SC2034
{
    if_info=$(ip addr show $i)
    echo $if_info | grep "UP.*mtu" -  &> /dev/null
    test1=$?
    echo $if_info | grep "inet " -  &> /dev/null
    test2=$?
    if [ $test1 -eq 0 ] && [ $test2 -eq 0 ] ; then
         # shellcheck disable=SC2016
        cmd='echo -n $(ifconfig $i 2>/dev/null | \grep inet|cut -d":" -f$index|cut -d" " -f1)'
        if [[ $get_ip -eq 1 ]]; then
            index=2
            eval $cmd
            exit 0
        fi
        if [[ $get_bcast -eq 1 ]]; then
            index=3
            # The screen scraping from ifconfig doesn't give a "good" broadcast
            # when we are the AP. It will give 0.0.0.0 instead of 255.255.255.0.
            # So we just force it to be 192.168.1.255 since it's almost always
            # what we need.
            output=`eval $cmd`
            if [[ $output == '0.0.0.0' ]]; then
                echo -n '192.168.1.255'
            else
                echo -n $output
            fi
            exit 0
        fi
        if [[ $get_mask -eq 1 ]]; then
            index=4
            eval $cmd
            exit 0
        fi
    fi
}
done

exit 1
