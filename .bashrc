# define PLATFORM & HOST
. $HOME/.myconfig

##########################################
# Functions
##########################################
# pip bash completion start
_pip_completion()
{
    COMPREPLY=( $( COMP_WORDS="${COMP_WORDS[*]}" \
                   COMP_CWORD=$COMP_CWORD \
                   PIP_AUTO_COMPLETE=1 $1 ) )
}
complete -o default -F _pip_completion pip

##########################################
# Configurations
##########################################

# shell options
shopt -s nocaseglob
stty -ixon

#export TERM=linux
#eval `/usr/bin/dircolors -b ~/.dircolors`

export TABSTOP=4
export PS1='[\w]$ ' 

source ~/.git-completion.bash
export GIT_PS1_SHOWDIRTYSTATE=1
export PS1='\[\e[32;1m\]$PLATFORM: \[\e[36;1m\]\w\[\e[32;1m\]$(__git_ps1 " (%s)")\[\e[0m\]>\n\$ '

. ~/.git-completion.bash

export PYTHONPATH=/usr/local/lib/python2.7/site-packages:$PYTHONPATH

. ~/.gph_alias

#############################################################
# Platform dependent
#############################################################

if [ $PLATFORM == 'MAC' ]; then
    export PATH=/usr/texbin:$PATH        # for brew
    export PATH=/usr/local/bin:$PATH        # for brew
    if [ -f $(brew --prefix)/etc/bash_completion ]; then
        . $(brew --prefix)/etc/bash_completion
    fi

    export PLATFORM # needed on MAC
fi
if [ $PLATFORM == 'LINUX' ]; then
    export EDITOR=vim

    if [ $HOST == 'UTRC' ]; then
        export PATH=$HOME/usr/bin:$PATH
        export PATH=/Users/gpenghe/QtSDK/Desktop/Qt/4.8.1/gcc/bin:$PATH
        export PATH=/usr/local/share/python:$HOME/usr/local/bin:$PATH
        export PATH=$PATH:/sbin:/usr/sbin
        export PATH=$HOME/bin:$PATH
        export PATH=$PATH:$HOME/.local/bin

        # csh copy of LD_LIBRARY_PATH
        export LD_LIBRARY_PATH=$LD_LIBRARY_PATH_COPY 

        # graphviz/dot
        #export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$HOME/usr/lib
        #export GVBINDIR=~/usr/lib

        # OpenMP (not used since set in csh)
        #export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/utrc/software/openmpi/1.6.5/gcc/lib
        export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$HOME/usr/local/lib
        export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$HOME/usr/lib
        # export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/utrc/fs1/software/python/2.7.3/lib


        export MANPATH=$MANPATH:$HOME/usr/man:$HOME/usr/local/man
        export MANPATH=/usr/local/opt/coreutils/libexec/gnuman:$MANPATH

        export PKG_CONFIG_PATH=$HOME/usr/local/lib/pkgconfig:$HOME/usr/lib/pkgconfig:$PKG_CONFIG_PATH
        export PKG_CONFIG_PATH=$HOME/ffmpeg_build/lib/pkgconfig:$PKG_CONFIG_PATH


        # INCLUDE && LIBDIR
        export CPATH=$CPATH:$HOME/usr/include:$HOME/usr/local/include
        export LIBPATH=$LIBPATH:$HOME/usr/lib:$HOME/usr/local/lib

        # for ./configure
        export CFLAGS="-I$HOME/usr/include -I$HOME/usr/local/include"
        export CPPFLAGS="$CFLAGS"
        export LDFLAGS="-L$HOME/usr/lib -L$HOME/usr/local/lib"
    fi  # LINUX - UTRC

    if [ -f ~/usr/etc/bash_completion ]; then
        . /usr/etc/bash_completion
    fi

fi

##########################################
# Web Services
##########################################
export IRCNICK=gossiper
export IRCSERVER=chat.freenode.net:6667:40964026
export CVS_RSH=ssh

#############################################################
# Misc
#############################################################
