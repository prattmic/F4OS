#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <dev/cortex_m.h>
#include <kernel/semaphore.h>
#include <kernel/sched.h>
#include <kernel/svc.h>
#include <kernel/fault.h>
#include "sched_internals.h"

extern int get_lock(volatile struct semaphore *semaphore);
extern void held_semaphores_remove(struct semaphore *list[], volatile struct semaphore *semaphore);

void systick_handler(void) __attribute__((section(".kernel")));
void pendsv_handler(void) __attribute__((section(".kernel")));
void svc_handler(uint32_t*) __attribute__((section(".kernel")));

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

static int svc_acquire(struct semaphore *semaphore) {
    int ret;

    if (get_lock(semaphore)) {
        /* Success */
        ret = 1;
    }
    else {
        /* Failure */
        ret = 0;

        if (task_exists(semaphore->held_by)
                && (task_compare(semaphore->held_by, curr_task) <= 0)) {
            get_task_ctrl(curr_task)->stack_top = PSP();
            switch_task(get_task_ctrl(semaphore->held_by));
        }
        else {
            /* If held_by didn't exist in task list,
             * it most likely just ended without releasing
             * the semaphore.  kernel_task should release
             * the semaphore on its behalf soon. */
            /* TODO: switch directly to kernel_task in this case */
            svc_yield();
        }
    }

    return ret;
}

void svc_release(struct semaphore *semaphore) {
    semaphore->lock = 0;
    semaphore->held_by = NULL;
    held_semaphores_remove(curr_task->semaphore_data.held_semaphores, semaphore);

    if (semaphore->waiting
            && (task_compare(semaphore->waiting, curr_task) >= 0)) {
        task_ctrl *task = get_task_ctrl(semaphore->waiting);
        semaphore->waiting = NULL;

        get_task_ctrl(curr_task)->stack_top = PSP();
        switch_task(task);
    }
}

void svc_handler(uint32_t *registers) {
    uint32_t svc_number;

    /* Stack contains:
     * r0, r1, r2, r3, r12, r14, the return address and xPSR
     * First argument and return value (r0) is registers[0] */
    svc_number = ((char *)registers[6])[-2];

    switch (svc_number) {
        case SVC_YIELD:
            svc_yield();
            break;
        case SVC_END_TASK:
            svc_end_task();
            break;
        case SVC_ACQUIRE:
            registers[0] = svc_acquire((struct semaphore *) registers[0]);
            break;
        case SVC_RELEASE:
            svc_release((struct semaphore *) registers[0]);
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
