#include "types.h"
#include "registers.h"
#include "interrupt.h"
#include "task.h"
#include "context.h"
#include "mem.h"
#include "semaphore.h"

void init_semaphore(volatile struct semaphore *semaphore) {
    semaphore->lock = 0;
    semaphore->held_by = NULL;
    semaphore->waiting = NULL;
}

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
    /* If in a fault, allow immediate use of semaphore */
    if (FAULTMASK()) {
        semaphore->lock = 1;
        /* This will cause a bus fault if someone tries to switch to it,
         * but in a fault state there should be no task switching */
        semaphore->held_by = (task_node *) 0xdefeca7e;
        return;
    }

    /* This a stupid hack to make GCC put curr_task
     * in a register before the branches below, which
     * screw with the optimizer */
    if (task_switching && !curr_task) {
        panic_print("Task switching, but no curr_task.");
    }

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
        /* Add to waitlist if higher priority */
        if (semaphore->waiting) {
            if (semaphore->waiting->task->priority < curr_task->task->priority) {
                semaphore->waiting = curr_task;
            }
        }
        else {
            semaphore->waiting = curr_task;
        }

        /* Swap or yield */
        if (semaphore->held_by->task->priority <= curr_task->task->priority) {
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
        /* Maybe we were interrupted and now it is available?  Try again */
        panic_print("Semaphore not available, but held_by unset.");
        __asm__("b try");
    }

    /********************/
    __asm__("success:":::"memory");
    semaphore->held_by = curr_task;
    return;
}
    
void release(volatile struct semaphore *semaphore) {
    semaphore->lock = 0;
    semaphore->held_by = NULL;

    if (task_switching && semaphore->waiting && semaphore->waiting->task->priority >= curr_task->task->priority) {
        task_node *task = semaphore->waiting;
        semaphore->waiting = NULL;
        swap_task(task);
    }
}
