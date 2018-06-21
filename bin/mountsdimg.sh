#!/usr/bin/env bash

img=$1
dev=$(sudo losetup --find --show --read-only --partscan $img)
[ -z $dev ] && exit 1
boot_dir="./boot"
root_dir="./root"
mkdir -p $boot_dir
mkdir -p $root_dir
sudo mount ${dev}p1 $boot_dir || exit 1
sudo mount ${dev}p2 $root_dir || exit 1
echo -e "\033[1;32mDone. BOOT partition under $boot_dir. ROOT partition under $root_dir\033[0m"
