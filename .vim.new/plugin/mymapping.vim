" Unmap Functions Keys
"-------------------------------------
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

" Map Functions Keys
"-------------------------------------
map  <F7>   :NERDTreeToggle<C-M>
map  <F5>	:bn<C-M>	" Some macros to manage the buffer of vim
map  <F6>	:bp<C-M>	" 
"map  <F7>	:cn<C-M>	" 
map  <F8>	:cp<C-M>	" 
map  <F9>   @a
"map <F1>0
"set pastetoggle=<F11>   " Not working
map <F11> :set invpaste<CR>

" Version Control System mappings
"-------------------------------------
map ,sD :vsp new<Enter>:read !svn diff<Enter>:set syntax=diff buftype=nofile<Enter>gg
map ,hD :vsp new<Enter>:read !hg diff<Enter>:set syntax=diff buftype=nofile<Enter>gg
map ,gD :vsp new<Enter>:read !git diff<Enter>:set syntax=diff buftype=nofile<Enter>gg

map ,sd :!svn cat -r BASE % > /tmp/tmpfile.c<Enter><Enter>:diffsplit /tmp/tmpfile.c<Enter><Enter>gg
map ,hd :!hg cat % > /tmp/tmpfile.c<Enter><Enter>:diffsplit /tmp/tmpfile.c<Enter><Enter>gg
map ,gd :!git show HEAD:% > /tmp/tmpfile.c<Enter><Enter>:diffsplit /tmp/tmpfile.c<Enter><Enter>gg

map ,ss :!svn stat\|grep -v "\.out$"\|grep -v "\.orig$"\|grep -v "\.swp$"<Enter>
map ,hs :!hg stat\|grep -v "\.out$"\|grep -v "\.orig$"\|grep -v "\.swp$"<Enter>
map ,gs :!git status -s<Enter>

map ,sl :!svn log\|head -n14<Enter>
map ,hl :!hg log\|head -n14<Enter>
map ,gl :!git tree2\|head -n14<Enter>

map ,hv :!hg view<Enter>
map ,gv :!gitk<Enter>

" Misc
"-------------------------------------
map ,e :vsp<Enter>:Explore<Enter>
inoremap {<cr> {<cr>}<c-o>O
inoremap [<cr> [<cr>]<c-o>O
inoremap (<cr> (<cr>)<c-o>O

map ,, :A<Enter>

map ZZ :wq<CR>
"map ,q :q<Enter>
"nnoremap ; :
"
