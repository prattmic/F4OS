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

#ifndef KERNEL_SCHED_INTERNALS_H_INCLUDED
#define KERNEL_SCHED_INTERNALS_H_INCLUDED

/*
 * Internal scheduler details that should not be needed outside of internal
 * scheduler files and arch-specific scheduler functions.
 */

#include <list.h>

typedef struct task_ctrl {
    uint32_t    *stack_limit;
    uint32_t    *stack_top;
    uint32_t    *stack_base;
    void        (*fptr)(void);
    uint32_t    period;
    uint32_t    ticks_until_wake;
    uint8_t     priority;
    uint8_t     running;
    uint8_t     abort;
    uint32_t    pid;
    struct list runnable_task_list;
    struct list periodic_task_list;
    struct list free_task_list;
    task_t      exported;
} task_ctrl;

/* Return task_ctrl struct given exported task_t address */
static inline __attribute__((always_inline)) struct task_ctrl *get_task_ctrl(task_t *task) {
    return container_of(task, struct task_ctrl, exported);
}

/* Return exportable task_t given task_ctrl address */
static inline __attribute__((always_inline)) task_t *get_task_t(task_ctrl *task) {
    return &task->exported;
}

/**
 * Scheduler service call handler
 *
 * Should only be called by global SVC handler.  Takes va_args for the given
 * service call number, and returns the result of the service call.
 *
 * @param svc_number    Service call number.  Must be a scheduler service call
 * @param va_args       Arguments for service call
 * @returns Return value of service call
 */
int sched_service_call(uint32_t svc_number, ...);

/**
 * Switch to given task
 *
 * Perform a task switch to the given task, or use the scheduling algorithm
 * to select a task.
 *
 * @param task  Task to switch to.  If NULL, use the scheduling algorithm to
 *              select a task.
 */
void switch_task(task_ctrl *task);

/**
 * Perform system tick of the RTOS
 *
 * This updates all periodic tasks timers, and should be called during each
 * system tick.
 */
void rtos_tick(void);

/*
 * Arch-provided functions
 */

/**
 * Save context on interrupt entry.
 *
 * Saves any additional context not saved by the processor when entering
 * interrupt space.
 */
void save_context(void);

/**
 * Restore context on interrupt exit.
 *
 * Restores any additional context not restored by the processor when exiting
 * interrupt space.
 */
void restore_context(void);

/**
 * Get userspace stack pointer
 *
 * Used for saving the SP in a context switch
 *
 * @returns Current userspace stack pointer
 */
uint32_t *get_user_stack_pointer(void);

/**
 * Set userspace stack pointer
 *
 * Set the userspace stack pointer before switching contexts to the task.
 * This stack will not become active until the actual context switch.
 *
 * @param stack  Stack address to set
 */
void set_user_stack_pointer(uint32_t *stack_addr);

/**
 * Create task context on the stack
 *
 * Creates the appropriate arch task context on the stack, which can be
 * restored to begin task execution.  Context is created on task's stack.
 *
 * @param task  Task to create context for.
 * @param lptr  Return address for task
 */
void create_context(task_ctrl *task, void (*lptr)(void));

#endif
