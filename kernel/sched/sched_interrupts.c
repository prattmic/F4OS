#include <stdarg.h>
#include <stdint.h>
#include <kernel/fault.h>
#include <kernel/sched.h>
#include <kernel/sched_internals.h>
#include "sched_internals.h"

int sched_service_call(uint32_t svc_number, ...) {
    int ret = 0;
    va_list ap;
    va_start(ap, svc_number);

    switch (svc_number) {
        case SVC_YIELD:
            sched_svc_yield();
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
