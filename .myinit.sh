cd $HOME
test -e .vim.new && echo "Error: the home directory has been initialized already!" && exit 1

sudo apt-get update
sudo apt-get install -y git
git clone https://github.com/jasongeng/linuxconfig

test -e .git && echo "Error: ~/.git already existed" && exit 1

cp -a linuxconfig/.git .
git reset --hard

echo "PLATFORM=UNKNOWN" >.myconfig
echo "HOST=UNKNOWN" >>.myconfig
ln -s .vim.new .vim
ln -s .vim/vimrc .vimrc

sudo apt-get install -y screen
sudo apt-get install -y vim

echo "Summary: installed:"
echo "git"
echo "vim"
echo "screen"

echo ""

echo "Next:"
echo "- Modify .myconfig"
echo "- Run :PlugInstall in vim"

