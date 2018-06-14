# method 1
sudo losetup -f -P -r --show imagename.img
losetup -l
sudo mount  /dev/loop0p2 p2
sudo mount  /dev/loop0p1 p1
sudo losetup -D

# method 2
sudo fdisk -l imagename.img
# take note of the START sector for the second partition
# Example:
#         Device Boot      Start         End      Blocks   Id  System
# imagename.img1            8192      122879       57344    c  W95 FAT32 (LBA)
# imagename.img2          122880    15415295     7646208   83  Linux
# 
# Then the start is 122880
sudo losetup /dev/loop0 imagename.img -o $((START*512))
