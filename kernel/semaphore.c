#include <stdint.h>
#include <stddef.h>
#include <dev/registers.h>
#include <kernel/sched.h>
#include <kernel/fault.h>

#include <kernel/semaphore.h>

void init_semaphore(volatile struct semaphore *semaphore) {
    semaphore->lock = 0;
    semaphore->held_by = NULL;
    semaphore->waiting = NULL;
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
    /* Local labels are numbers like 0 or 1. A branch to 1b goes "back" to last
     * 1, and 1f goes "forward" to the next 1. These are necessary because the
     * optimizer will inline this code at random, and labels will be redefined */
    __asm__("0:");
    __asm__("\
            mov         r2, #1              \r\n\
            ldrexb      r3, [%[addr]]       \r\n\
            cmp         r3, #0              \r\n\
            ITT         EQ                  \r\n\
            strexbeq    r1, r2, [%[addr]]   \r\n\
            cmpeq       r1, #0              \r\n\
            bne         1f                  \r\n\
            b           2f                  \r\n\
        1:"
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
        __asm__("b 0b");
    }
    else {
        /* How was the semaphore taken, yet no one holds it? */
        /* Maybe we were interrupted and now it is available?  Try again */
        //panic_print("Semaphore not available, but held_by unset.");
        __asm__("b 0b");
    }

    /********************/
    __asm__("2:");
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
/* --- Work in progress, pure asm rewrite of acquire, to fix weird stuff the optimizer does because we branch between asm blocks ---
void acquire(volatile struct semaphore *semaphore) {
    __asm__("\
            ldr         r4, [%[sem_addr]]       \r\n\
            ldr         r1, [%[ts_addr]]        \r\n\
            cmp         r1, #0                  \r\n\
            bne         try                     \r\n\
            movw        r2, #51838              \r\n\
            mowt        r2, #57086              \r\n\
            str         r2, [r4, #4]            \r\n\
            b           ret                     \r\n\
        try:                                    \r\n\
            mov         r2, #1                  \r\n\
            ldrexb      r3, [%[sem_addr]]       \r\n\
            cmp         r3, #0                  \r\n\
            ITT         EQ                      \r\n\
            strexbeq    r1, r2, [%[sem_addr]]   \r\n\
            cmpeq       r1, #0                  \r\n\
            bne         giveup                  \r\n\
            b           success                 \r\n\
        giveup:"
            ::[sem_addr] "l"(&semaphore->lock), [ts_addr] "m" (&task_switching)
            :"r1", "r2", "r3", "cc", "memory");

    if (semaphore->held_by != NULL) {
        if (semaphore->waiting) {
            if (semaphore->waiting->task->priority < curr_task->task->priority) {
                semaphore->waiting = curr_task;
            }
        }
        else {
            semaphore->waiting = curr_task;
        }

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
        //panic_print("Semaphore not available, but held_by unset.");
        __asm__("b try");
    }

    __asm__("success:");
    semaphore->held_by = curr_task;
    return;
}
*/  

