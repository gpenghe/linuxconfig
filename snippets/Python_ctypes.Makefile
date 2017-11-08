all:
	gcc -Wall -Werror -shared -Wl,-soname,testlib -o testlib.so -fPIC testlib.c
