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

#include <dev/hw/systick.h>
#include <kernel/fault.h>

#include <kernel/sched.h>
#include <kernel/sched_internals.h>
#include "sched_internals.h"

volatile uint8_t task_switching = 0;
task_t * volatile curr_task;

static void start_task_switching(void) __attribute__((section(".kernel")));

void start_sched(void) {
    /* Set up initial tasks */
    new_task(&kernel_task, 10, 4);
    new_task(&sleep_task, 0, 0);

    /* Setup boot tasks specified by end user. */
    main();

    init_systick();
    start_task_switching();
}

static void start_task_switching(void) {
    if (list_empty(&runnable_task_list)) {
        panic_print("No tasks to run!");
    }

    struct list *element = runnable_task_list.next;

    task_ctrl *task = list_entry(element, task_ctrl, runnable_task_list);

    curr_task = get_task_t(task);

    //mpu_stack_set(task->stack_base);

    task_switching = 1;
    task->running = 1;

    create_context(task, &end_task);

    set_user_stack_pointer(task->stack_top);
    restore_full_context();
}

