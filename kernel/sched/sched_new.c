#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list.h>
#include <mm/mm.h>
#include <dev/resource.h>
#include <kernel/fault.h>

#include <kernel/sched.h>
#include "sched_internals.h"

static task_ctrl *create_task(void (*fptr)(void), uint8_t priority, uint32_t period) __attribute__((section(".kernel")));
static int register_task(task_ctrl *task_ptr, int periodic) __attribute__((section(".kernel")));

struct list runnable_task_list = INIT_LIST(runnable_task_list);
struct list periodic_task_list = INIT_LIST(periodic_task_list);

DEFINE_INSERT_TASK_FUNC(runnable_task_list);
DEFINE_INSERT_TASK_FUNC(periodic_task_list);

void new_task(void (*fptr)(void), uint8_t priority, uint32_t period) {
    task_ctrl *task = create_task(fptr, priority, period);
    if (task == NULL) {
        goto fail;
    }

    int ret = register_task(task, period);
    if (ret != 0) {
        goto fail2;
    }

    total_tasks += 1;

    return;

fail2:
    free(task->stack_limit);
    kfree(task);
fail:
    panic_print("Could not allocate task with function pointer 0x%x", fptr);
}

void create_context(task_ctrl* task, void (*lptr)(void)) {
    asm volatile("mov     r5, #0                                                              \n\
                  stmdb   %[stack]!, {r5}   /* Empty */                                       \n\
                  stmdb   %[stack]!, {r5}   /* FPSCR */                                       \n\
                  stmdb   %[stack]!, {r5}   /* S15 */                                         \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}   /* S0 */                                          \n\
                  ldr.w   r5, =0x01000000 /* Thumb state bit must be set */                   \n\
                  stmdb   %[stack]!, {r5}   /* xPSR */                                        \n\
                  mov     r5, #0                                                              \n\
                  stmdb   %[stack]!, {%[pc]}/* PC */                                          \n\
                  stmdb   %[stack]!, {%[lr]}/* LR */                                          \n\
                  stmdb   %[stack]!, {r5}   /* R12 */                                         \n\
                  stmdb   %[stack]!, {r5}   /* R3 */                                          \n\
                  stmdb   %[stack]!, {r5}   /* R2 */                                          \n\
                  stmdb   %[stack]!, {r5}   /* R1 */                                          \n\
                  stmdb   %[stack]!, {r5}   /* R0 */                                          \n\
                  stmdb   %[stack]!, {r5}   /* R11 */                                         \n\
                  stmdb   %[stack]!, {r5}   /* R10 */                                         \n\
                  stmdb   %[stack]!, {r5}   /* R9 */                                          \n\
                  stmdb   %[stack]!, {r5}   /* R8 */                                          \n\
                  stmdb   %[stack]!, {%[frame]}   /* R7 - Frame Pointer*/                     \n\
                  stmdb   %[stack]!, {r5}   /* R6 */                                          \n\
                  stmdb   %[stack]!, {r5}   /* R5 */                                          \n\
                  stmdb   %[stack]!, {r5}   /* R4 */                                          \n\
                  stmdb   %[stack]!, {r5}   /* S31 */                                         \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}   /* S16 */"
                  :[stack] "+r" (task->stack_top) /* Output */
                  :[pc] "r" (task->fptr), [lr] "r" (lptr), [frame] "r" (task->stack_limit)   /* Input */
                  :"r5"   /* Clobber */);

}

static task_ctrl *create_task(void (*fptr)(void), uint8_t priority, uint32_t period) {
    task_ctrl *task;
    uint32_t *memory;
    static uint32_t pid_source = 1;
    task = (task_ctrl *) kmalloc(sizeof(task_ctrl));
    if (task == NULL) {
        return NULL;
    }

    memory = (uint32_t *) malloc(STKSIZE*4);
    if (memory == NULL) {
        kfree(task);
        return NULL;
    }

    task->stack_limit       = memory;
    task->stack_base        = memory + STKSIZE;
    task->stack_top         = memory + STKSIZE;
    task->fptr              = fptr;
    task->priority          = priority;
    task->running           = 0;
    task->abort             = 0;

    task->period            = period;
    task->ticks_until_wake  = period;
    task->pid               = pid_source++;

    list_init(&task->runnable_task_list);
    list_init(&task->periodic_task_list);
    list_init(&task->free_task_list);

    resource_setup(task);
    memset(task->held_semaphores, 0, sizeof(task->held_semaphores));
    task->waiting           = NULL;

    return task;
}

static int register_task(task_ctrl *task, int periodic) {
    /* When task switching, we cannot safely modify the task lists
     * ourselves, instead we must ask the OS to do so for us. */
    if (task_switching) {
        SVC_ARG2(SVC_REGISTER_TASK, task, periodic);
    }
    else {
        _register_task(task, periodic);
    }

    return 0;
}

void _register_task(task_ctrl *task, int periodic) {
    insert_task(runnable_task_list, task);

    if (periodic) {
        insert_task(periodic_task_list, task);
    }
}
