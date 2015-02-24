#!/bin/sh

#rsync -avhx -AESX --delete --progress --hfs-compression --protect-decmpfs /Volumes/SG1T\ -\ Backup/iPhoto\ Library/ /Volumes/SG1T\ -\ Time\ Machine/iPhoto-rsync-Backup -n # Test
rsync -avhx -AESX --delete --progress --hfs-compression --protect-decmpfs /Volumes/SG1T\ -\ Backup/iPhoto\ Library/ /Volumes/SG1T\ -\ Time\ Machine/iPhoto-rsync-Backup # Action
