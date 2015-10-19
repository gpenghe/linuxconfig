inoremap {<cr> {<cr>}<c-o>O
inoremap [<cr> [<cr>]<c-o>O
inoremap (<cr> (<cr>)<c-o>O

map ,, :A<Enter>
map ,q :q<Enter>
map ,hv :!hg view<Enter>
map ,sD :vsp new<Enter>:read !svn diff<Enter>:set syntax=diff buftype=nofile<Enter>gg
map ,hD :vsp new<Enter>:read !hg diff<Enter>:set syntax=diff buftype=nofile<Enter>gg
"FIXME: file name processing
"map ,d :tmpfile=tempname().".c"<Enter><Enter>:!svn cat -r BASE % > tmpfile<Enter><Enter>:vert diffsplit tmpfile<Enter><Enter>:set buftype=nofile<Enter><Enter>gg
map ,sd :!svn cat -r BASE % > /tmp/tmpfile.c<Enter><Enter>:diffsplit /tmp/tmpfile.c<Enter><Enter>gg
map ,hd :!hg cat % > /tmp/tmpfile.c<Enter><Enter>:diffsplit /tmp/tmpfile.c<Enter><Enter>gg
map ,ss :!svn stat\|grep -v "\.out$"\|grep -v "\.orig$"\|grep -v "\.swp$"<Enter>
map ,hs :!hg stat\|grep -v "\.out$"\|grep -v "\.orig$"\|grep -v "\.swp$"<Enter>
map ,ss :!svn stat\|grep -v "\.out$"\|grep -v "\.orig$"\|grep -v "\.swp$"<Enter>
map ,sl :!svn log\|head -n14<Enter>
map ,hl :!hg log\|head -n14<Enter>
map ,e :vsp<Enter>:Explore<Enter>
map ZZ :wq<CR>
map zz :q<CR>

nnoremap ; :
" 2009-0507
"nn <C-w> 4w
"nn <C-b> 4b
nn <C-h> 3h
nn <C-l> 3l
"nn <C-k> 3k
"nn <C-j> 3j
"nn <C-S-o> o<ESC>
"
"" Switch between windows, maximizing the current window
"
"map <C-J> <C-W>j<C-W>_
"map <C-K> <C-W>k<C-W>_ 
map <C-h> :ConqueTermVSplit bash<CR>

"------------- Functions Keys -----------
" Unmapp all function keys
map <F1>	<ESC>
map <F2>	<ESC>
map <F3>	<ESC>
map <F4>	<ESC>
map <F5>	<ESC>
map <F6>	<ESC>
map <F7>	<ESC>
map <F8>	<ESC>
map <F9>	<ESC>
map <F10>	<ESC>
map <F11>	<ESC>
map <F12>	<ESC>
imap <F1>	<ESC>
imap <F2>	<ESC>
imap <F3>	<ESC>
imap <F4>	<ESC>
imap <F5>	<ESC>
imap <F6>	<ESC>
imap <F7>	<ESC>
imap <F8>	<ESC>
imap <F9>	<ESC>
imap <F10>	<ESC>
imap <F11>	<ESC>
imap <F12>	<ESC>

map  <F5>	:bn<C-M>	" Some macros to manage the buffer of vim
map  <F6>	:bp<C-M>	" 
map  <F7>	:cn<C-M>	" 
map  <F8>	:cp<C-M>	" 
map  <F9>   @a

"=== toggle paste
map <F10> :set paste<CR>
map <F11> :set nopaste<CR>
imap <F10> <C-O>:set paste<CR>
imap <F11> <nop>
set pastetoggle=<F11>

"map <C-S-C> :s/\(^[[:blank:]]*\)/\1#/<ESC>:noh<ESC>j
"map <C-S-D> :s/\(^[[:blank:]]*\)#/\1/<ESC>:noh<ESC>j

