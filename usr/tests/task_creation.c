#include <kernel/sched.h>
#include <string.h>
#include "test.h"

volatile int task_created = 0;

static void basic_task(void) {
    task_created = 1;
}

int task_creation(char *message, int len) {
    new_task(&basic_task, 5, 0);

    /* Wait a while for task to be created */
    int count = 100000;
    while (count-- && !task_created);

    if (task_created) {
        return PASSED;
    }

    strncpy(message, "Task creation timed out", len);
    return FAILED;
}
DEFINE_TEST("Task creation", task_creation);
