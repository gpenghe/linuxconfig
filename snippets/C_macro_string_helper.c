#define TEST 2

#include <stdio.h>

int main(void) {
#define STR(x) STR_HELPER(x)
#define STR_HELPER(x) #x
    printf(STR(TEST)"\n");
    printf(STR_HELPER(TEST)"\n");
    return 0;
}
