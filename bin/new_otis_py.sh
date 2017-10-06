newfile=$1
test -e $newfile && echo "Error: $newfile already exists" && exit 1

echo "Creating $newfile with Otis Python header line"
touch $newfile
echo "# Copyright 2017 Otis Elevator Co." > $newfile
echo "" >> $newfile
echo "" >> $newfile
