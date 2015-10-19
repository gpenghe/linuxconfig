set tags=./tags;/

if filereadable(expand(".vimrc.local"))
    source .vimrc.local
elseif filereadable(expand("../.vimrc.local"))
    source ../.vimrc.local
elseif filereadable(expand("../../.vimrc.local"))
    source ../../.vimrc.local
elseif filereadable(expand("../../../.vimrc.local"))
    source ../../../.vimrc.local
elseif filereadable(expand("../../../../.vimrc.local"))
    source ../../../../.vimrc.local
elseif filereadable(expand("../../../../../.vimrc.local"))
    source ../../../../../.vimrc.local
elseif filereadable(expand("../../../../../../.vimrc.local"))
    source ../../../../../../.vimrc.local
endif

"cs add /home/gph/cscope/realsrc.out
"cs add /home/gph/cscope/realsrc.out2
"cs add /home/gph/platform/BF561/hhbf/uClinux-dist/linux-2.6.x/cscope.out
"mplayer:
"cs add /home/gph/cscope/mplayer-1.0pre8.out
"cs add /home/gph/cscope/mplayer-official-svn.out
"cs add /home/gph/cscope/mplayer-bfin.out

"if $CSCOPE_DB != ""
"    if $CONSOLE == "CYGWIN"
"        cs add $CSCOPE_DB
"    endif
"endif

"let tmpf = "../cscope.out"
"if filereadable(expand(tmpf))
"  exec "cs add ". tmpf
"endif
"
"let tmpf = "../../cscope.out"
"if filereadable(expand(tmpf))
"  exec "cs add ". tmpf
"endif
"
"let tmpf = "../../../cscope.out"
"if filereadable(expand(tmpf))
"  exec "cs add ". tmpf
"endif
"
"let tmpf = "../../../../cscope.out"
"if filereadable(expand(tmpf))
"  exec "cs add ". tmpf
"endif
"
"let tmpf = "../../../../../cscope.out"
"if filereadable(expand(tmpf))
"  exec "cs add ". tmpf
"endif


