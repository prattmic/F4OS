#include <stddef.h>
#include <kernel/fault.h>

#include <kernel/sched.h>
#include <kernel/sched_internals.h>
#include "sched_internals.h"

void switch_task(task_ctrl *task) {
    /* Optionally pass task to switch to, otherwise pass NULL */

    /* Rate monotonic scheduling
     * Always runs the highest priority task,
     * which will be the head of the list, as
     * it is kept sorted.  Round-robin through
     * equal priority tasks. */

    if (task == NULL) {
        if (list_empty(&runnable_task_list)) {
            /* Uh-oh, no tasks! */
            panic_print("No tasks to run.");
        }

        struct list *element = list_pop_head(&runnable_task_list);
        task = list_entry(element, task_ctrl, runnable_task_list);

        curr_task = get_task_t(task);

        /* As a workaround for lack of MPU support, check if the
         * stack of the task we are switching from has overflowed */
        if (task->stack_limit > task->stack_top) {
            panic_print("Task (0x%x, fptr: 0x%x) has overflowed its stack. "
                        "stack_top: 0x%x stack_limit: 0x%x", task, task->fptr,
                        task->stack_top, task->stack_limit);
        }

        insert_task(runnable_task_list, task);
    }
    else {
        curr_task = get_task_t(task);
    }

    /* mpu_stack_set(task->stack_base);   Sigh...maybe some day */

    if (!task->running) {
        task->running = 1;
        create_context(task, &end_task);
    }

    set_user_stack_pointer(task->stack_top);
}

/* Switch to task if it exists.
 * NULL task is equivalent to yield.
 * Return 0 on success */
int svc_task_switch(task_ctrl *task) {
    if (task && !task_runnable(get_task_t(task))) {
        return -1;
    }

    get_task_ctrl(curr_task)->stack_top = get_user_stack_pointer();
    switch_task(task);
    return 0;
}

/* Updates ticks in all periodic tasks */
void rtos_tick(void) {
    task_ctrl *task;

    list_for_each_entry(task, &periodic_task_list, periodic_task_list) {
        if (task->ticks_until_wake == 0) {
            if (!task->running) {
                insert_task(runnable_task_list, task);
            }
            task->ticks_until_wake = task->period;
        }
        else {
            task->ticks_until_wake--;
        }
    }
}
