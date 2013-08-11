#ifndef KERNEL_SCHED_H_INCLUDED
#define KERNEL_SCHED_H_INCLUDED

#include <stdint.h>
#include <dev/resource.h>
#include <kernel/semaphore.h>
#include <kernel/svc.h>

/* Boolean field indicating whether or not the scheduler
 * has begun task switching. */
extern volatile uint8_t task_switching;

/* Type used to refer to tasks from outside the scheduler.
 * Given a pointer to a task_t, the scheduler should be able
 * to uniquely identify a task.  This type also includes
 * all generic task infomation that must be available outside
 * the scheduler. */
typedef struct task_t {
    struct task_resource_data   resource_data;
    struct task_semaphore_data  semaphore_data;
} task_t;

/* Unique identifier of the currently executing task */
extern task_t * volatile curr_task;

/* Total number of tasks currently running.  This does
 * not include periodic tasks that are waiting to be
 * run. */
extern volatile uint32_t total_tasks;

/* Initialize the scheduler and begin task switching.
 * This function should not return. */
void start_sched(void);

/* Create a new task in the scheduler.  This task consists
 * of a function pointer, a priority, and a period.
 * Non-periodic tasks use a period of zero.
 * Depending on the scheduler, priority and period may
 * have no effect. Returns task_t reference to new task. */
task_t *new_task(void (*fptr)(void), uint8_t priority, uint32_t period);

/* End-users set up boot tasks here.
 * This function will be run before scheduling starts, and
 * should be used to create the tasks that should run when
 * scheduling begins. */
void main(void);

/* Only perform a yield if task switching is active, and we are
 * not in an interrupt context */
static inline void yield_if_possible(void) __attribute__((always_inline));
static inline void yield_if_possible(void) {
    if (task_switching && arch_svc_legal()) {
        SVC(SVC_YIELD);
    }
}

/* Do non-scheduler setup for new task.
 * This function should be called by any scheduler implementation. */
void generic_task_setup(task_t *task);

/* Task comparison.
 * Compares two tasks in terms of priority, if applicable to the scheduler
 * Returns 0 for equality, >0 if task1 is greater, <0 if task2 is greater */
int task_compare(task_t *task1, task_t *task2);

/* Determine if a task is runnable.
 * Returns >0 if task is runnable, 0 if not.
 * A task may not be runnable because it doesn't exist,
 * or because it is in an unrunnable state (such as a periodic
 * task between runs). */
uint8_t task_runnable(task_t *task);

/* Switch to task
 * Immediately switches to task, as long as it is running.
 * Passing the NULL task is equivalent to yielding.
 * Returns zero on success, non-zero on error (generally because task is
 * not runnable) */
int task_switch(task_t *task);

#endif
