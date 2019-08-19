#!/bin/bash

echo Testing reading speed from $1
if [[ -z $1 ]]; then
    echo Usage example: $0 /dev/sda
else
    disk=$1
    sudo dd if=$disk of=/dev/null bs=1M count=20k
fi
