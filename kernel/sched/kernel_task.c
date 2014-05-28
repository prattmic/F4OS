/*
 * Copyright (C) 2013, 2014 F4OS Authors
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
#include <kernel/power.h>
#include <kernel/fault.h>
#include <kernel/mutex.h>
#include <kernel/sched.h>
#include <kernel/sched_internals.h>
#include "sched_internals.h"

struct list free_task_list = INIT_LIST(free_task_list);

void kernel_task(void) {
    /* Does cleanup that can't be done from outside a task (ie. in an interrupt) */

    while (!list_empty(&free_task_list)) {
        struct list *element = list_pop(&free_task_list);
        struct task_ctrl *task = list_entry(element, struct task_ctrl, free_task_list);

        /* Free abandoned mutexes */
        for (int i = 0; i < HELD_MUTEXES_MAX; i++) {
            struct task_mutex_data *mut_data = &get_task_t(task)->mutex_data;

            if (mut_data->held_mutexes[i]) {
                release(mut_data->held_mutexes[i]);
            }
        }

        free_task(task);
    }
}

void sleep_task(void) {
    /* Run when there is nothing else to run */
    while (1) {
        arch_wait_for_interrupt();
    }
}
