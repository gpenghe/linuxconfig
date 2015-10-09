command D :call PRINTDATE()
command Pd :call PRINTDATE()
	func PRINTDATE() "type current date
	read !date +\%Y.\%m.\%d
	endf

command Dc :call DCOMM()
	func DCOMM() " delete the block comment macro lines.
	exec "normal 1l"
	let l1 = searchpair('^\s*#\s*if\s\+\d\+', '', '^\s*#\s*endif', 'Wb')
	if l1 < 1
	return
	endif
	exec "normal ]#"
	if getline(".") =~ '^\s*#\s*else'
	return
	endif
	exec "normal dd" . l1 . "Gdd"
	endf

command Rc :call RCOMM()
	func RCOMM()  " reverse the block comment.
	exec "normal 1l"
	if searchpair('^\s*#\s*if\s\+\d\+', '', '^\s*#\s*enif', 'Wb') < 1
	return
	endif
	exec '.s#\d\+#\=submatch(0)==0 ? 1 : 0#'
	endf

command -range Co :call  COMM(<line1>,<line2>)
	func COMM(l1, l2) " add the MACRO comment around the block of C/Cpp code.
	exec a:l2+1 . "s%^%#endif%"
	exec a:l1 . "s%^%#ifndef BS_NO_8BIT /* HHTECH, %"
	exec "normal k"
	exec "Pd"
	exec "normal kJ"
	exec a:l1 . "s%$% */%"
	endf

command -range Ch :call  CHANGE(<line1>,<line2>)
	func CHANGE(l1, l2) " add the if/else/endif comment around the block of C/Cpp code.
	call COMM(a:l1,a:l2)
	exec a:l2+2 . "s%^%#else%"
	exec "normal k"
	endf

command -range Sh :call  ADDSHARP(<line1>,<line2>)
	func ADDSHARP(l1, l2) " add # before each line 
	exec a:l1 . "," . a:l2 . "s%^%#%"
	endf

function MyConqueStartup(term)

    " set buffer syntax using the name of the program currently running
    let syntax_associations = { 'ipython': 'python', 'irb': 'ruby' }

    if has_key(syntax_associations, a:term.program_name)
        execute 'setlocal syntax=' . syntax_associations[a:term.program_name]
    else
        execute 'setlocal syntax=' . a:term.program_name
    endif

    " shrink window height to 10 rows
    vertical resize 90

    " silly example of terminal api usage
    "if a:term.program_name == 'bash'
        "call a:term.writeln('svn up ~/projects/*')
    "endif

endfunction

call conque_term#register_function('after_startup', 'MyConqueStartup')

