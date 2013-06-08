#include <stddef.h>
#include <stdlib.h>
#include <mm/mm.h>
#include <kernel/fault.h>

#include <kernel/sched.h>
#include "sched_internals.h"

void free_task(task_ctrl *task) {
    free(task->stack_limit);
    kfree(task);
}

/* Abort a periodic task */
void abort(void) {
    curr_task->abort = 1;
    SVC(SVC_END_TASK);    /* Shouldn't return (to here, at least) */
}

void end_task(void) {
    SVC(SVC_END_TASK);    /* Shouldn't return (to here, at least) */
}

/* Called by svc_handler */
void svc_end_task(void) {
    if (curr_task->stack_limit > curr_task->stack_top) {
        panic_print("Task (0x%x, fptr: 0x%x) has overflowed its stack. stack_top: 0x%x stack_limit: 0x%x", curr_task, curr_task->fptr, curr_task->stack_top, curr_task->stack_limit);
    }

    list_remove(&curr_task->runnable_task_list);

    /* Periodic (but only if aborted) */
    if (curr_task->period && curr_task->abort) {
        list_remove(&curr_task->periodic_task_list);
    }

    /* Periodic (but only if not aborted) */
    if (!curr_task->abort && curr_task->period) {
        curr_task->running = 0;

        /* Reset stack */
        curr_task->stack_top = curr_task->stack_base;
    }
    else {
        /* Add to queue for freeing */
        list_add(&curr_task->free_task_list, &free_task_list);

        total_tasks -= 1;
    }

    switch_task(NULL);
}
