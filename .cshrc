setenv PATH "${PATH}:${HOME}/bin:/sbin:/usr/sbin" 
setenv PATH "${HOME}/opt/localgit/bin:${PATH}"
#setenv PATH "/usr/local/lsf/7.0/linux2.6-glibc2.3-x86_64/bin:${PATH}"
source /utrc/fs1/login/Software/lsf_env
source /utrc/fs1/login/Software/openmpi163_env intel12
setenv LD_LIBRARY_PATH_COPY $LD_LIBRARY_PATH


