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

#include <kernel/svc.h>
#include <kernel/sched.h>
#include <kernel/sched_internals.h>
#include "sched_internals.h"

/* Implements general scheduler API functions */

int task_compare(task_t *task1, task_t *task2) {
    /* Anything is better than the NULL task */
    if (!task1) {
        return -1;
    }

    if (!task2) {
        return 1;
    }

    struct task_ctrl *t1 = get_task_ctrl(task1);
    struct task_ctrl *t2 = get_task_ctrl(task2);

    if (t1->priority > t2->priority) {
        return 1;
    }
    else if (t2->priority > t1->priority) {
        return -1;
    }

    return 0;
}

uint8_t task_runnable(task_t *task) {
    task_ctrl *match = get_task_ctrl(task);
    task_ctrl *node;

    list_for_each_entry(node, &runnable_task_list, runnable_task_list) {
        if (node == match) {
            return 1;
        }
    }

    return 0;
}

int task_switch(task_t *task) {
    int ret;
    task_ctrl *t = task ? get_task_ctrl(task) : NULL;

    /* If possible, make a service call */
    if (arch_svc_legal()) {
        ret = SVC_ARG(SVC_TASK_SWITCH, t);
    }
    /* Otherwise, switch directly */
    else {
        ret = svc_task_switch(t);
    }

    return ret;
}
