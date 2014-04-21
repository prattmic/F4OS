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

#include <stdarg.h>
#include <stdint.h>
#include <time.h>
#include <kernel/fault.h>
#include <kernel/sched.h>
#include <kernel/sched_internals.h>
#include "sched_internals.h"

void sched_system_tick(void) {
    system_ticks++;

    /* Update periodic tasks */
    rtos_tick();

    /* Run the scheduler */
    task_switch(NULL);
}

int sched_service_call(uint32_t svc_number, ...) {
    int ret = 0;
    va_list ap;
    va_start(ap, svc_number);

    switch (svc_number) {
        case SVC_YIELD:
            svc_task_switch(NULL);
            break;
        case SVC_END_TASK:
            sched_svc_end_task();
            break;
        case SVC_REGISTER_TASK: {
            task_ctrl *task = va_arg(ap, task_ctrl *);
            int periodic = va_arg(ap, int);
            svc_register_task(task, periodic);
            break;
        }
        case SVC_TASK_SWITCH: {
            task_ctrl *task = va_arg(ap, task_ctrl *);
            ret = svc_task_switch(task);
            break;
        }
        default:
            panic_print("Unknown SVC: %d", svc_number);
            break;
    }

    va_end(ap);

    return ret;
}
