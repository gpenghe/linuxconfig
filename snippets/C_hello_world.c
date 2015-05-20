#include <stdio.h>
#include <stdlib.h>

#define FUNC_START() printf("DEBUG: %s entered.\n", __FUNCTION__)
#define FUNC_END() printf("DEBUG: %s exiting.\n", __FUNCTION__)

void func3()
{
    FUNC_START();
    
    FUNC_END();
}

void func2()
{
    FUNC_START();

    func3();
    
    FUNC_END();
}

void func1()
{
    FUNC_START();

    func2(); 

    FUNC_END();
}

int main(int argc, char** argv)
{
    printf("Hello, world!\n");
    printf("---------------\n");

    func1();
    printf("---------------\n");

    func2();
    printf("---------------\n");

    func3();
    printf("---------------\n");

    return 0;
}
