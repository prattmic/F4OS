/*
 * Copyright (C) 2013 F4OS Authors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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

int svc_task_switch(task_ctrl *task) {
    if (task && !task_runnable(get_task_t(task))) {
        return -1;
    }

    /*
     * The scheduler is started by initiating a task switch from
     * start_sched(), before task_switching is set.  In this case,
     * perform the final setup before switching to the first task.
     * Do not save the current task's stack pointer, since there is
     * no current task.
     */
    if (!task_switching) {
        /*
         * Begin preemptive system ticks
         * Done in interrupt context to ensure that the first scheduler
         * preemptive interrupt cannot occur until switching has actually
         * begun (upon return from this interrupt).
         */
        arch_sched_start_system_tick();

        task_switching = 1;
    }
    else {
        get_task_ctrl(curr_task)->stack_top = get_user_stack_pointer();
    }

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
