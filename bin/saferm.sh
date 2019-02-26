#!/bin/bash

has_important_file=0
use_rf=0

for item in $*; do
	if [ $item = '-rf' ]; then
		use_rf=1;
    fi
    if [[ $item == "build" ]] || [[ $item == "build/" ]] || [ -d ${item}/.git ]; then
		has_important_file=1;
        important_files="$item $important_files"
	fi
done

if [ $has_important_file -eq 1 ] && [ $use_rf -eq 1 ]; then  # Doing the confirmation
	echo "Do you really want to delete: $important_files?"
	echo -n "Type YES to confirm: "
	read input
	if [[ -n $input ]] && [[ $input == "YES" ]]; then
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
