#include <stdint.h>
#include <kernel/fault.h>
#include <kernel/sched.h>
#include <kernel/semaphore.h>

void svc_handler(uint32_t*) __attribute__((section(".kernel")));

void svc_handler(uint32_t *registers) {
    uint32_t svc_number;

    /* Stack contains:
     * r0, r1, r2, r3, r12, r14, the return address and xPSR
     * First argument and return value (r0) is registers[0] */
    svc_number = ((char *)registers[6])[-2];

    switch (svc_number) {
        case SVC_YIELD:
        case SVC_END_TASK:
        case SVC_REGISTER_TASK:
        case SVC_TASK_SWITCH:
            svc_sched(svc_number, registers);
            break;
        case SVC_ACQUIRE:
        case SVC_RELEASE:
            svc_semaphore(svc_number, registers);
            break;
        default:
            panic_print("Unknown SVC: %d", svc_number);
            break;
    }
}
