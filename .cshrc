#!/bin/csh 
# ~/.cshrc
# 
# This is a standard .cshrc. Do not modify.
# Modify the file /home/<USER>/.cshrc.user.<USER>

# Determine system type:

     if ( ! $?SYSTEM ) then
        set SYSTEM = "`uname -s`"
     endif
     if ( ! $?REV ) then
        set REV = "`uname -r`"
     endif
     if ( ! $?VER ) then
        set VER = "`uname -v`"
     endif
  
     if ( $SYSTEM == "SunOS" ) then
        if ( $REV =~ 5.[4-8] ) then
           set SYSTEM = Solaris
        endif
     else if ( $SYSTEM == "AIX" ) then
        set SYSTEM = "AIX$VER.$REV"
     else if ( $SYSTEM == "IRIX64") then
        set SYSTEM = "IRIX64.$REV"
     else if ( $SYSTEM == "IRIX") then
        set SYSTEM = "IRIX.$REV"
     endif
#-----------------------------------------
# OS specific path, man path, libpath
	if ( -f /utrc/fs1/login/$SYSTEM/.path.system.$SYSTEM ) then
        source /utrc/fs1/login/$SYSTEM/.path.system.$SYSTEM
     endif
	else if ( -f $SYSTEM == "Linux") then
		if ( -f /.path.system.$SYSTEM ) then
			source /.path.system.$SYSTEM
		endif
	endif

# Host specific path, man path, libpath
     if ( -f /.path.host.`hostname` ) then
        source /.path.host.`hostname`
     endif

# User specific path, man path, libpath
     if ( -f ~/.path.user.$USER ) then
        source ~/.path.user.$USER
     endif

#-----------------------------------------
# Set up environment for various codes
# Place AFTER default paths have been setup

     source /utrc/fs1/login/Software/master_software_env

#-----------------------------------------
# Stop here if not interactive session
#     tty -s >& /dev/null
#     if ($status) then
#        exit 0
#     endif

# Set up default environment
#     if ( -f /.environment.default) then
	if ( -f /utrc/fs1/login/$SYSTEM/.environment.default ) then
        source /utrc/fs1/login/$SYSTEM/.environment.default
     endif

# Source user's file

     if ( -f ~/.cshrc.user.$USER ) then
        source ~/.cshrc.user.$USER
     endif

