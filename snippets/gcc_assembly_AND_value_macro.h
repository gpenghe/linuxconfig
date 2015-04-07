#ifndef SP_UTIL_H
#define SP_UTIL_H

#define GET_SP() \
({                                          \
    uint64_t tmpsp;                         \
    asm                                     \
        (                                   \
         "mov %%rsp, %0\n\t"                \
         : "=r" (tmpsp)                     \
         : /* no input */                   \
        );                                  \
    tmpsp;                                  \
 })

#endif
