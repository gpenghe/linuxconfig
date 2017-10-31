#!/bin/bash
# Scan 255.255.255.0 subnet for active IPs

# NOT WORKING! -- 2017-10-31

if [ $# -eq 0 ]; then 
    host_ip=`print_ip`
else
    host_ip=$1
fi

echo Scanning network for IP: $host_ip
echo -----------

ip_range=`echo $host_ip |sed 's#\(\.\)[0-9][0-9]*$#\10/24#'` # like 192.168.7.0/24

ip_list=`nmap -n -sn $ip_range |             \
    sed -e '/^$/d'                  \
    -e '/^Starting/d'               \
    -e '/^Host/d'                   \
    -e '/^Nmap done/d'              \
    -e '/MAC Address/d'             \
    | awk '{print $5;}' | sort -V | xargs`

echo $ip_list

for ip in  $ip_list; do
    if ! `nslookup -timeout=0.1 $ip | grep '\.lan\.' > /dev/null`; then
        name="Unknown_Host"
    else
        name=`nslookup -timeout=1 $ip | grep '\.lan\.' | cut -d= -f 2 | cut -d. -f 1`
    fi
    echo -e "$ip \t$name" 
done

echo -----------

