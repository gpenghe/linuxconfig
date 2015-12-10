autocmd BufEnter *.cpp,*.c,*.h map <C-S-C> :s/\(^[[:blank:]]*\)/\1\/\//<ESC>:noh<ESC>j
autocmd BufEnter *.cpp,*.c,*.h map <C-S-D> :s/\(^[[:blank:]]*\)\/\//\1/<ESC>:noh<ESC>j      
autocmd BufEnter *.json set syntax=javascript
autocmd BufEnter *.pde set syntax=java

au BufNewFile,BufRead *.asm,*.equ,*.INC    setf picasm

"au BufNewFile,BufRead README			setf rst
"au BufNewFile,BufRead *.txt			setf rst
