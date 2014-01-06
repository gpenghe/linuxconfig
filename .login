#
# created 10/26/2010 GG

if ($?ARCH == 0 ) then
    setenv ARCH linux
endif

# set LSF path

source /utrc/fs1/login/Software/lsf_env

# GG 01/27/2011 Only reset keyboard if this is a tty session
tty -s >& /dev/null
if (! $status) then
        stty erase "^H" intr "^C"
endif

