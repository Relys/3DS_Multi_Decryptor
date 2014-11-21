#ifndef _3DS_H
#define _3DS_H

static inline int FlushProcessDataCache(uint32_t process, void *addr, uint32_t size)
{
    register long r0 asm ("r0") = process;
    register void *r1 asm ("r1") = addr;
    register long r2 asm ("r2") = size;
    asm volatile ( "SVC 0x54" : "=r"(r0) : "r"(r0), "r"(r1), "r"(r2) );
    return r0;
}

#endif
