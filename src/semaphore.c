#include "types.h"
#include "registers.h"
#include "interrupt.h"
#include "task.h"
#include "context.h"
#include "mem.h"
#include "semaphore.h"

/* --- Basically the same as the locking example in ARM docs --- */
void spin_acquire(volatile struct semaphore *semaphore) {
    __asm__("mov         r2, #1            \n\t"
            "spin:                         \n\t"
            "ldrexb      r3, [%[addr]]     \n\t"
            "cmp         r3, #0            \n\t"
            "ITT         EQ                \n\t"
            "strexbeq    r1, r2, [%[addr]] \n\t"
            "cmpeq       r1, #0            \n\t"
            "bne         spin              \n\t"
            ::[addr] "l"(semaphore)
            :"r1", "r2", "r3", "cc", "memory");
}

void acquire(volatile struct semaphore *semaphore) {
    /* If in a fault, allow immediate use of semaphore */
    if (FAULTMASK()) {
        semaphore->lock = 1;
        /* This will cause a bus fault if someone tries to switch to it,
         * but in a fault state there should be no task switching */
        semaphore->held_by = (task_node *) 0xdefeca7e;
        return;
    }

    __asm__("try: \n\t"
            "mov         r2, #1              \n\t"
            "ldrexb      r3, [%[addr]]       \n\t"
            "cmp         r3, #0              \n\t"
            "ITT         EQ                  \n\t"
            "strexbeq    r1, r2, [%[addr]]   \n\t"
            "cmpeq       r1, #0              \n\t"
            "bne         giveup              \n\t"
            "b           success             \n\t"
            "giveup:                         \n\t"
            ::[addr] "l"(&semaphore->lock)
            :"r1", "r2", "r3", "cc", "memory");

    if (semaphore->held_by != NULL) {
        if (semaphore->held_by->task->priority <= curr_task->task->priority) {
            swap_task(semaphore->held_by);
        }
        else {
            __asm__("svc    %[yield] \n\t"
                     ::[yield] "I"(SVC_YIELD):);
        }
        __asm__("b try \n\t");
    }
    else {
        /* How was the semaphore taken, yet no one holds it? */
        /* Maybe we were interrupted and now it is available?  Try again */
        /* panic_print("Semaphore not available, but held_by unset.");*/
        __asm__("b try \n\t");
    }

    /********************/
    __asm__("success: \n\t");
    semaphore->held_by = curr_task;
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
