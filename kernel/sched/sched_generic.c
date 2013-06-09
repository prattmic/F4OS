#include <kernel/sched.h>
#include <kernel/semaphore.h>
#include <dev/resource.h>

/* Functions common to all scheduler implementations */

/* Do non-scheduler setup for new task */
void generic_task_setup(task_t *task) {
    task_resource_setup(task);
    task_semaphore_setup(task);
}
