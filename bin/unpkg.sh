password="46PNZBC7BsugAzMFPdns"

d=$(basename $1 .pkg)
test -e $d && rm -rf $d
mkdir $d
cd $d
unzip -P $password ../$1
tar xzvf ics_kinect.tgz
rm -f ics_kinect.tgz
