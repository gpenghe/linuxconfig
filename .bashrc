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

function prompt_command {
    local ret="$?"
    errmsg=''
    [[ $ret -ne 0 ]] && errmsg="->($(printf '%x' $ret))"
    if [[ $(uname) == 'Darwin' ]]; then
        ut=$(uptime | awk '{print $10}')
    else
        ut=$(uptime |sed -e 's/.*: //' -e 's/,.*//')
    fi
    tm=$(date +"%m-%d %H:%M:%S")
}

if [[ "$SHELL" =~ "bash" ]]; then
    complete -o default -F _pip_completion pip
    # shell options
    shopt -s nocaseglob
    export GIT_PS1_SHOWDIRTYSTATE=1
    export GIT_PS1_SHOWUNTRACKEDFILES=1
    export GIT_PS1_SHOWSTASHSTATE=1
    export GIT_PS1_SHOWUPSTREAM=auto
    PROMPT_COMMAND=prompt_command
    PS1='\[\e[32;1m\]$(hostname):\e[36;1m $tm\e[34;1m $ut \[\e[36;1m\]\w\[\e[32;1m\]$(__git_ps1 " %s")\[ \e[36;1m>\e[31;1m$errmsg\e[0m\]\n\$ '
    source ~/.git-completion.bash
fi


#eval `/usr/bin/dircolors -b ~/.dircolors`

export TABSTOP=4
# export PYTHONPATH=/usr/local/lib/python2.7/site-packages:$PYTHONPATH

export PATH=$PATH:$HOME/bin
export EDITOR=vim
export VISUAL=vim

#############################################################
# Setting Aliases
#############################################################

. $HOME/.gph_alias

#############################################################
# Platform dependent
#############################################################

# 
# Removing the following completely since it's causing an warning on Ubuntu:
# "stty: 'standard input': Inappropriate ioctl for device"
#if [ $HOST != "MINIPC" ] && [ $HOST != "MAC_LINUX" ]; then
#    stty -ixon
#fi

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
    export TERM=linux

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
    elif [ $HOST = 'MINIPC' ] || [ $HOST = 'MAC_LINUX' ]; then
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
    else
        [[ -f /etc/bash_completion ]] && . /etc/bash_completion
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
if [ -e $HOME/.bashrc.local ]; then
    . $HOME/.bashrc.local
fi

export PATH="$PATH:$HOME/.rvm/bin" # Add RVM to PATH for scripting

. $HOME/.bash_completion

# disable the following since it's too slow (~500ms)
# export NVM_DIR="$HOME/.nvm"
# [ -s "$NVM_DIR/nvm.sh" ] && \. "$NVM_DIR/nvm.sh"  # This loads nvm
# [ -s "$NVM_DIR/bash_completion" ] && \. "$NVM_DIR/bash_completion"  # This loads nvm bash_completion


# Eternal bash history.
# ---------------------
# Undocumented feature which sets the size to "unlimited".
# http://stackoverflow.com/questions/9457233/unlimited-bash-history
export HISTFILESIZE=
export HISTSIZE=
export HISTTIMEFORMAT="[%F %T] "
# Change the file location because certain bash sessions truncate .bash_history file upon close.
# http://superuser.com/questions/575479/bash-history-truncated-to-500-lines-on-each-login
export HISTFILE=~/.bash_eternal_history
# Force prompt to write history after every command.
# http://superuser.com/questions/20900/bash-history-loss
export PROMPT_COMMAND="${PROMPT_COMMAND}; history -a; history -c; history -r;"

# fuzzy finder for Ctrl-R
[ -f ~/.fzf.bash ] && source ~/.fzf.bash
export FZF_DEFAULT_COMMAND='fd --type f'

# fuzzy cd for j
# on Ubuntu:
[[ -s $HOME/.autojump/etc/profile.d/autojump.sh ]] && source $HOME/.autojump/etc/profile.d/autojump.sh
# on Arch Linux
[[ -s /etc/profile.d/autojump.sh ]] && source /etc/profile.d/autojump.sh


# last line
true  # always returns 0
