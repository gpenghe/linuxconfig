# define PLATFORM
. $HOME/.myconfig
export PLATFORM # needed on MAC

#mkdir /r/cygwin_tmp &>/dev/null

#shell options
shopt -s nocaseglob
stty -ixon

#export TERM=linux
#eval `/usr/bin/dircolors -b ~/.dircolors`

export TABSTOP=4
export PS1='[\w]$ ' 

source ~/.git-completion.bash
export GIT_PS1_SHOWDIRTYSTATE=1
export PS1='\[\e[32;1m\]$PLATFORM: \[\e[36;1m\]\w\[\e[32;1m\]$(__git_ps1 " (%s)")\[\e[0m\]>\n\$ '

export IRCNICK=gossiper
export IRCSERVER=chat.freenode.net:6667:40964026
export CVS_RSH=ssh

export PATH=$HOME/bin:$PATH
export PATH=$HOME/usr/bin:$PATH
export PATH=/Users/gpenghe/QtSDK/Desktop/Qt/4.8.1/gcc/bin:$PATH
export PATH=/usr/local/share/python:$HOME/usr/local/bin:$PATH
export PATH=$PATH:/sbin:/usr/sbin
export PATH=/utrc/fs1/software/python/2.7.3/bin:$PATH
#export PATH=$PATH:/utrc/software/openmpi/1.6.5/gcc/bin

# csh copy of LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH_COPY 

# graphviz/dot
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$HOME/usr/lib
export GVBINDIR=~/usr/lib

# OpenMP (not used since set in csh)
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/utrc/software/openmpi/1.6.5/gcc/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$HOME/usr/local/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/utrc/fs1/software/python/2.7.3/lib


export MANPATH=/usr/local/opt/coreutils/libexec/gnuman:$MANPATH

export PKG_CONFIG_PATH=$HOME/usr/local/lib/pkgconfig:$HOME/usr/lib/pkgconfig:$PKG_CONFIG_PATH
export PKG_CONFIG_PATH=$HOME/ffmpeg_build/lib/pkgconfig:$PKG_CONFIG_PATH

if [ -f ~/usr/etc/bash_completion ]; then
    . /usr/etc/bash_completion
fi

. ~/.git-completion.bash

export PYTHONPATH=/usr/local/lib/python2.7/site-packages:$PYTHONPATH

# pip bash completion start
_pip_completion()
{
    COMPREPLY=( $( COMP_WORDS="${COMP_WORDS[*]}" \
                   COMP_CWORD=$COMP_CWORD \
                   PIP_AUTO_COMPLETE=1 $1 ) )
}
complete -o default -F _pip_completion pip
# pip bash completion end

#############################################################
# Platform dependent
#############################################################
if [ $PLATFORM == 'MAC' ]; then
    export PATH=/usr/texbin:$PATH        # for brew
    export PATH=/usr/local/bin:$PATH        # for brew
    if [ -f $(brew --prefix)/etc/bash_completion ]; then
        . $(brew --prefix)/etc/bash_completion
    fi
fi
. ~/.gph_alias
export http_proxy=http://proxyva.utc.com:8080
export ftp_proxy=http://proxyva.utc.com:8080

