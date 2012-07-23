#include "types.h"
#include "registers.h"
#include "interrupt.h"
#include "task.h"
#include "context.h"
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
    __asm__("try:");
    __asm__("\
            mov         r2, #1              \r\n\
            ldrexb      r3, [%[addr]]       \r\n\
            cmp         r3, #0              \r\n\
            ITT         EQ                  \r\n\
            strexbeq    r1, r2, [%[addr]]   \r\n\
            cmpeq       r1, #0              \r\n\
            bne         giveup              \r\n\
            b           success             \r\n\
        giveup:"
            ::[addr] "l"(&semaphore->lock)
            :"r1", "r2", "r3", "cc", "memory");

    if (semaphore->held_by != NULL) {
        if (semaphore->held_by->task->priority <= k_curr_task->task->priority) {
            swap_task(semaphore->held_by);
        }
        else {
            __asm__("svc    %[yield]"
                     ::[yield] "I"(SVC_YIELD):);
        }
        __asm__("b try");
    }
    else {
        /* How was the semaphore taken, yet no one holds it? */
        panic();
    }

    /********************/
    __asm__("success:");
    semaphore->held_by = k_curr_task;
    return;
}
    
void release(volatile struct semaphore *semaphore) {
    semaphore->lock = 0;
    semaphore->held_by = NULL;
}

void init_semaphore(volatile struct semaphore *semaphore) {
    semaphore->lock = 0;
    semaphore->held_by = NULL;
}
