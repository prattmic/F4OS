#include "types.h"
#include "registers.h"
#include "context.h"
#include "task.h"
#include "mem.h"
#include "semaphore.h"

/* --- Basically the same as the locking example in ARM docs --- */
void spin_acquire(volatile struct semaphore *semaphore) {
    __asm__("\
            mov         r2, #1\r\n              \
        spin:                                   \
            ldrexb      r3, [%[addr]]\r\n       \
            cmp         r3, #0\r\n              \
            ITT         EQ\r\n                  \
            strexbeq    r1, r2, [%[addr]]\r\n   \
            cmpeq       r1, #0\r\n              \
            bne         spin\r\n"
            ::[addr] "l"(semaphore)
            :"r1", "r2", "r3", "cc", "memory");
}

void acquire(volatile struct semaphore *semaphore) {
    __asm__("\
            mov         r2, #1\r\n              \
        try:                                    \
            ldrexb      r3, [%[addr]]\r\n       \
            cmp         r3, #0\r\n              \
            ITT         EQ\r\n                  \
            strexbeq    r1, r2, [%[addr]]\r\n   \
            cmpeq       r1, #0\r\n              \
            bne         giveup\r\n              \
            bx          lr\r\n                  \
        giveup:                                 \
            svc         %[yieldnum]\r\n         \
            b           try"
            ::[addr] "l"(semaphore), [yieldnum] "I"(SVC_YIELD)
            :"r1", "r2", "r3", "cc", "memory");
}
    
void release(volatile struct semaphore *semaphore) {
    semaphore->lock = 0;
    semaphore->held_by = NULL;
}

void init_semaphore(volatile struct semaphore *semaphore) {
    semaphore->lock = 0;
    semaphore->held_by = NULL;
}
