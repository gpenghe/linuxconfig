if [ -z "$PROJ" ]; then
    echo "Must define PROJ with one of the following:"
    echo "      TrustedComputing"
    echo "      enics"
    exit -1
fi

if [ $# == 0 ]; then
    echo "Usage: share.sh <files>"
    exit -1
fi

echo Sharing "$*" to $PROJ
git_dir=$HOME/shared-data/$PROJ
if ! [ -d "$git_dir" ]; then
    echo "$git_dir doesn't exist."
    exit -1
fi

cp -a $* $git_dir
cd $git_dir
git add --all .
git commit -m "New shared data: $*"

