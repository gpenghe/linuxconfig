#include <stdio.h>
#include <stdint.h>

int main(void) {
    struct Foo {
        uint8_t c;
        uint32_t n;
    } __attribute__ ((packed)) foo;

    uint8_t* p = (uint8_t*)&foo;
    p[0] = 1;
    p[1] = 2;
    p[2] = 3;
    p[3] = 4;
    p[4] = 5;

    printf("sizeof Foo:%u\n", sizeof(struct Foo));
    printf("Foo.n: 0x%08X\n", foo.n);
    uint32_t* pn = (uint32_t*)&foo.n;
    printf("Foo.n (Direct): 0x%08X (at %p)\n", *pn, pn);
    return 0;
}
