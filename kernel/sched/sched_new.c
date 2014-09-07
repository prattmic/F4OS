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

#include <math.h>
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
#include <kernel/sched_internals.h>
#include "sched_internals.h"

struct list runnable_task_list = INIT_LIST(runnable_task_list);
struct list periodic_task_list = INIT_LIST(periodic_task_list);

DEFINE_INSERT_TASK_FUNC(runnable_task_list);
DEFINE_INSERT_TASK_FUNC(periodic_task_list);

volatile uint32_t total_tasks = 0;

static task_ctrl *create_task(void (*fptr)(void), uint8_t priority,
                              uint32_t period) {
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

    generic_task_setup(get_task_t(task));

    return task;
}

static int register_task(task_ctrl *task, int periodic) {
    /* When task switching, we cannot safely modify the task lists
     * ourselves, instead we must ask the OS to do so for us. */
    if (task_switching) {
        SVC_ARG2(SVC_REGISTER_TASK, task, periodic);
    }
    else {
        svc_register_task(task, periodic);
    }

    return 0;
}

task_t *new_task(void (*fptr)(void), uint8_t priority, uint32_t period_us) {
    uint32_t tick_period_us, period_ticks;
    task_ctrl *task;

    /* Tick period in us / tick */
    tick_period_us = 1000*1000 / CONFIG_SYSTICK_FREQ;

    /*
     * Round ticks up, ensuring that short period tasks are not
     * treated as non-periodic, if even one tick is too long.
     */
    period_ticks = DIV_ROUND_UP(period_us, tick_period_us);

    task = create_task(fptr, priority, period_ticks);
    if (task == NULL) {
        goto fail;
    }

    int ret = register_task(task, period_ticks);
    if (ret != 0) {
        goto fail2;
    }

    total_tasks += 1;

    return get_task_t(task);

fail2:
    free(task->stack_limit);
    kfree(task);
fail:
    panic_print("Could not allocate task with function pointer 0x%x", fptr);
}

void svc_register_task(task_ctrl *task, int periodic) {
    insert_task(runnable_task_list, task);

    if (periodic) {
        insert_task(periodic_task_list, task);
    }
}
