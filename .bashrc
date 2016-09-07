# define PLATFORM & HOST
# PLATFORM could be:
#       * WIN
#       * MAC
#       * LINUX
. $HOME/.myconfig
if [ -z "$PLATFORM" ] || [ -z "$HOST" ]; then
    printf '$PLATFORM and $HOST must be both defined in ~/.myconfig'
fi

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

##########################################
# Configurations
##########################################

if [[ "$SHELL" =~ "bash" ]]; then
    complete -o default -F _pip_completion pip
    # shell options
    shopt -s nocaseglob
    if [ $PLATFORM = 'CYGWIN' ]; then
        PS1='[\w]$ '
    else
        PS1='\[\e[32;1m\]$HOST: \[\e[36;1m\]\w\[\e[32;1m\]$(__git_ps1 " (%s)")\[\e[0m\]>\n\$ '
    fi
    export GIT_PS1_SHOWDIRTYSTATE=1
    source ~/.git-completion.bash
fi


#eval `/usr/bin/dircolors -b ~/.dircolors`

export TABSTOP=4
export PYTHONPATH=/usr/local/lib/python2.7/site-packages:$PYTHONPATH

export PATH=$PATH:$HOME/bin

export TERM=linux
. ~/.gph_alias

#############################################################
# Platform dependent
#############################################################

if [ $HOST != "MINIPC" ] && [ $HOST != "MAC_LINUX" ]; then
    stty -ixon      # Somehow it's causing Ubuntu an error dialog
fi

if [ $PLATFORM = 'MAC' ]; then
    export EDITOR=vim
    export TERM=xterm-256color
    export PATH=/usr/texbin:$PATH        # for brew
    export PATH=/usr/local/bin:$PATH        # for brew
    export PATH=/usr/local/texlive/2013/bin/x86_64-darwin:$PATH
    if [[ "$SHELL" =~ "bash" ]]; then
        if [ -f $(brew --prefix)/etc/bash_completion ]; then
            . $(brew --prefix)/etc/bash_completion
        fi
    fi
    export PATH=$PATH:/Applications/Postgres.app/Contents/Versions/latest/bin
    export PLATFORM # needed on MAC
fi

if [ $PLATFORM = 'LINUX' ]; then
    export EDITOR=vim
    export SHELL="/bin/bash"

    if [ $HOST = 'UBUNTU_VM' ]; then
        # for TechNexion Android build
        PATH="/home/gph/perl5/bin${PATH+:}${PATH}"; export PATH;
        PERL5LIB="/home/gph/perl5/lib/perl5${PERL5LIB+:}${PERL5LIB}"; export PERL5LIB;
        PERL_LOCAL_LIB_ROOT="/home/gph/perl5${PERL_LOCAL_LIB_ROOT+:}${PERL_LOCAL_LIB_ROOT}"; export PERL_LOCAL_LIB_ROOT;
        PERL_MB_OPT="--install_base \"/home/gph/perl5\""; export PERL_MB_OPT;
        PERL_MM_OPT="INSTALL_BASE=/home/gph/perl5"; export PERL_MM_OPT;

        export PATH=$PATH:/opt/android-ndk-r7c

        export USE_CCACHE=1
        export SHELL=/bin/sh
    fi

    if [ $HOST = 'MINIPC' ] || [ $HOST = 'MAC_LINUX' ]; then
        # For Java
        export JDK_HOME=$HOME/opt/jdk
        export JAVA_HOME=$HOME/opt/jdk

        # For Android SDK/NDK
        export ANDROID_SDK=$HOME/Android/Sdk # Auto-installed by Android Studio
        export ANDROID_NDK=$HOME/opt/android-ndk
        export PATH=$PATH:$HOME/opt/android-studio/bin
        export PATH=$PATH:$ANDROID_SDK/tools
        export PATH=$PATH:$ANDROID_SDK/platform-tools
        export PATH=$PATH:$ANDROID_NDK

        . /etc/bash_completion
    fi

    if [ $HOST = 'UTRC' ]; then
        export PATH=$HOME/usr/bin:$PATH
        export PATH=/Users/gpenghe/QtSDK/Desktop/Qt/4.8.1/gcc/bin:$PATH
        export PATH=/usr/local/share/python:$HOME/usr/local/bin:$PATH
        export PATH=$PATH:/sbin:/usr/sbin
        export PATH=$PATH:$HOME/.local/bin

        # csh copy of LD_LIBRARY_PATH
        export LD_LIBRARY_PATH=$LD_LIBRARY_PATH_COPY 
        export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$HOME/usr/lib
        export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$HOME/usr/local/lib
        # export TERM=vt320       # to fix vim
        export TERM=xterm       # to fix vim

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

    if [[ "$SHELL" =~ "bash" ]]; then
        if [ -f $HOME/usr/etc/bash_completion ]; then
            . $HOME/usr/etc/bash_completion
        fi
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
if [ -e $HOME/.bashrc.host ]; then
    . $HOME/.bashrc.host
fi

export PATH="$PATH:$HOME/.rvm/bin" # Add RVM to PATH for scripting
