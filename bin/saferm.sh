#!/bin/sh

#echo $0 $*

has_important_file=0;
use_rf=0
protect=0
hgfolder=0;

for item in $*; do
	if [ $item = '-rf' ]; then
		use_rf=1;
	fi
	if [ ${item%%hg} != $item ] || [ ${item%%ckup} != $item ] || [ -d ${item}/.hg ]; then
		has_important_file=1;
		if [ -z $important_file ]; then
			important_file="$item"
		else
			important_file="$item $important_file"
		fi
		if [ -d $item/.hg ]; then
			hgfolder=1;
		fi
	fi
done

if [ $has_important_file -eq 1 ] && [ $use_rf -eq 1 ]; then
	protect=1;
fi

if [ $protect -eq 1 ]; then # Doing the confirmation
	[ ${hgfolder:-0} -eq 1 ] && echo "$important_file contain(s) .hg!"
	echo "Do you really want to delete: $important_file?"
	echo -n "Type YES to confirm: "
	read input;
	if [ -n $input ] && [ $input = "YES" ]; then
		echo rm -i $*
		rm -i $*
	else
		echo "Not doing rm..."
		exit 1;
	fi
else # No need to protect
	echo rm -i $*
	rm -i $*
fi

		
