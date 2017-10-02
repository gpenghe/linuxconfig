#!/bin/bash

date=$(date +"%Y-%m-%d_%H-%M-%S")
test -e $HOME/Backup || test -e $HOME/backup || mkdir $HOME/Backup
test -e $HOME/Backup && backupdir=$HOME/Backup || backupdir=$HOME/backup
test "$1" || { echo "Must provide an argument" ; exit 1; }
source=$1
name=$(basename $source)  # remove the possible trailing "/"
target="$backupdir/${name}_$date.tgz"
echo Backing up $source to $target ...
tar -czf "$target" "$source"
echo Done
