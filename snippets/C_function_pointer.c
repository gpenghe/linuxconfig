#include <stdio.h>
#include <stdarg.h>

typedef void (*func)(void);
typedef void (*print_func)(const char* format, ...);

typedef struct func_struct {
    func mfunc;
    print_func pfunc;
} func_struct;

void f(void){
    printf("in f(void)\n");
}

void pf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

int main(void) {
    func_struct fs;
    fs.mfunc = f;
    fs.pfunc = pf;
    fs.mfunc();
    fs.pfunc("test: %d, %s\n", 8, "hehe");
    return 0;
}
