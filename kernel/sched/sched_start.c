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

#include <compiler.h>
#include <kernel/sched.h>
#include <kernel/sched_internals.h>
#include "sched_internals.h"

volatile uint8_t task_switching = 0;

/*
 * "Task" before task switching begins
 *
 * Used for accessing things like stdin/stdout before
 * task switching begins.
 */
static struct task_ctrl pre_switch_task;
task_t * volatile curr_task = &pre_switch_task.exported;

void start_sched(void) {
    /*
     * Set up initial tasks.
     * Kernel task performs cleanup every millisecond.
     */
    new_task(&kernel_task, 10, 1000);
    new_task(&sleep_task, 0, 0);

    /* Setup boot tasks specified by end user. */
    main();

    /* Perform last minute arch setup */
    arch_sched_start_bootstrap();

    /* Switch to first task */
    task_switch(NULL);
}

/* By default, do nothing */
void __weak arch_sched_start_bootstrap(void) {}
