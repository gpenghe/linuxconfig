#!/bin/bash -x

date=$(date +"%Y-%m-%d_%H-%M-%S")
backupdir="$HOME/Backup"
test "$1" || { echo "Must provide an argument" ; exit 1; }
source=$1
name=$(basename $source)  # remove the possible trailing "/"
target="$backupdir/${name}_$date.tgz"
echo Backing up $source to $target ...
tar -czf "$target" "$source"
echo Done
