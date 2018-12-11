#include <stdio.h>

void main() {
	char *args[5];

	args[0] = "/usr/bin/env";
	args[1] = "echo";
	args[2] = "hello";
	args[3] = "world";
	args[4] = NULL;
	execve(args[0], args, NULL);
}
