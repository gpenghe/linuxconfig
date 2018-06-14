img=$1
dev=$(losetup --find --show --read-only --partscan $img)
mount ${dev}p1 /mnt/sd/boot
mount ${dev}p2 /mnt/sd/rootfs

