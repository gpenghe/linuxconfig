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
export PS1='\[\e[36;1m\]\w\[\e[32;1m\]$(__git_ps1 " (%s)")\[\e[0m\]>\n\$ '

export IRCNICK=gossiper
export IRCSERVER=chat.freenode.net:6667:40964026
export CVS_RSH=ssh

export PATH=$HOME/bin:$PATH
export PATH=/Users/gpenghe/QtSDK/Desktop/Qt/4.8.1/gcc/bin:$PATH
export PATH=/usr/local/share/python:$PATH
#export PATH=/usr/local/opt/coreutils/libexec/gnubin:$PATH

export MANPATH=/usr/local/opt/coreutils/libexec/gnuman:$MANPATH

if [ -f ~/usr/etc/bash_completion ]; then
    . /usr/etc/bash_completion
fi

alias gv='git log --oneline --graph --decorate --all'
alias gs='git status'
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
. $HOME/.myconfig
if [ $PLATFORM == 'MAC' ]; then
    export PATH=/usr/texbin:$PATH        # for brew
    export PATH=/usr/local/bin:$PATH        # for brew
    if [ -f $(brew --prefix)/etc/bash_completion ]; then
        . $(brew --prefix)/etc/bash_completion
    fi
fi
. ~/.gph_alias


