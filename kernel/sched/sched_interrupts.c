#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <dev/cortex_m.h>
#include <kernel/semaphore.h>
#include <kernel/sched.h>
#include <kernel/fault.h>
#include "sched_internals.h"

extern int get_lock(volatile struct semaphore *semaphore);
extern void held_semaphores_remove(struct semaphore *list[], volatile struct semaphore *semaphore);

void systick_handler(void) __attribute__((section(".kernel")));
void pendsv_handler(void) __attribute__((section(".kernel")));
void tim2_handler(void) __attribute__((section(".kernel")));
void svc_handler(uint32_t*) __attribute__((section(".kernel")));

void systick_handler(void) {
    system_ticks++;

    /* Update periodic tasks */
    rtos_tick();

    /* Call PendSV to do switching */
    *SCB_ICSR |= SCB_ICSR_PENDSVSET;
}

void pendsv_handler(void){
    curr_task->task->stack_top = PSP();

    switch_task(NULL);
}

void svc_yield(void) {
    curr_task->task->stack_top = PSP();
    switch_task(NULL);
}

void svc_acquire(uint32_t *registers) {
    struct semaphore *semaphore = (struct semaphore *) registers[0];

    if (get_lock(semaphore)) {
        /* Success */
        registers[0] = 1;
    }
    else {
        /* Failure */
        registers[0] = 0;

        if (task_exists(semaphore->held_by) && semaphore->held_by->task->priority <= curr_task->task->priority) {
            curr_task->task->stack_top = PSP();
            switch_task(semaphore->held_by);
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
}

void svc_release(uint32_t *registers) {
    struct semaphore *semaphore = (struct semaphore *) registers[0];

    semaphore->lock = 0;
    semaphore->held_by = NULL;
    held_semaphores_remove(curr_task->task->held_semaphores, semaphore);

    if (semaphore->waiting && semaphore->waiting->task->priority >= curr_task->task->priority) {
        task_node *task = semaphore->waiting;
        semaphore->waiting = NULL;

        curr_task->task->stack_top = PSP();
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
            svc_acquire(registers);
            break;
        case SVC_RELEASE:
            svc_release(registers);
            break;
        default:
            panic_print("Unknown SVC");
            break;
    }
}
