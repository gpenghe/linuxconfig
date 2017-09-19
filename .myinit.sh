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
esst_apps="diffstat colordiff ctags"
esst_pips="netifaces"    # for print_ip
sudo apt-get install -y $esst_apps
sudo pip install $esst_pips

echo "Summary:"
"Installed apps:"
echo git vim screen $esst_apps
"Installed pip packages:"
echo $esst_pips

echo ""

echo "Next:"
echo "- Modify .myconfig"
echo "- Run :PlugInstall in vim"

