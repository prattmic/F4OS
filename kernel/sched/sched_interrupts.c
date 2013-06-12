#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <dev/cortex_m.h>
#include <kernel/semaphore.h>
#include <kernel/sched.h>
#include <kernel/svc.h>
#include <kernel/fault.h>
#include "sched_internals.h"

void systick_handler(void) __attribute__((section(".kernel")));
void pendsv_handler(void) __attribute__((section(".kernel")));

void systick_handler(void) {
    system_ticks++;

    /* Call PendSV to do switching */
    *SCB_ICSR |= SCB_ICSR_PENDSVSET;
}

void pendsv_handler(void){
    /* Update periodic tasks */
    rtos_tick();

    get_task_ctrl(curr_task)->stack_top = PSP();

    switch_task(NULL);
}

static void svc_yield(void) {
    get_task_ctrl(curr_task)->stack_top = PSP();
    switch_task(NULL);
}

void svc_sched(uint32_t svc_number, uint32_t *registers) {
    if (!IPSR()) {
        panic_print("Attempted to call service call from user space");
    }

    switch (svc_number) {
        case SVC_YIELD:
            svc_yield();
            break;
        case SVC_END_TASK:
            svc_end_task();
            break;
        case SVC_REGISTER_TASK:
            _register_task((task_ctrl *) registers[0], (int) registers[1]);
            break;
        case SVC_TASK_SWITCH:
            registers[0] = coop_task_switch((task_ctrl *) registers[0]);
            break;
        default:
            panic_print("Unknown SVC: %d", svc_number);
            break;
    }
}
