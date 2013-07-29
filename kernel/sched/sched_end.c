#include <stddef.h>
#include <stdlib.h>
#include <mm/mm.h>
#include <kernel/fault.h>

#include <kernel/sched.h>
#include <kernel/sched_internals.h>
#include "sched_internals.h"

void free_task(task_ctrl *task) {
    free(task->stack_limit);
    kfree(task);
}

/* Abort a periodic task */
void abort(void) {
    struct task_ctrl *task = get_task_ctrl(curr_task);

    task->abort = 1;

    SVC(SVC_END_TASK);    /* Shouldn't return (to here, at least) */
}

void end_task(void) {
    SVC(SVC_END_TASK);    /* Shouldn't return (to here, at least) */
}

/* Called by svc_handler */
void sched_svc_end_task(void) {
    struct task_ctrl *task = get_task_ctrl(curr_task);

    if (task->stack_limit > task->stack_top) {
        panic_print("Task (0x%x, fptr: 0x%x) has overflowed its stack. stack_top: 0x%x stack_limit: 0x%x", task, task->fptr, task->stack_top, task->stack_limit);
    }

    list_remove(&task->runnable_task_list);

    /* Periodic (but only if aborted) */
    if (task->period && task->abort) {
        list_remove(&task->periodic_task_list);
    }

    /* Periodic (but only if not aborted) */
    if (!task->abort && task->period) {
        task->running = 0;

        /* Reset stack */
        task->stack_top = task->stack_base;
    }
    else {
        /* Add to queue for freeing */
        list_add(&task->free_task_list, &free_task_list);

        total_tasks -= 1;
    }

    switch_task(NULL);
}
