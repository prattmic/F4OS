#include <stdio.h>
#include <kernel/sched.h>
#include "test.h"

volatile task_t *t1 = NULL;
volatile task_t *t2 = NULL;

volatile int t1_done = 0;
volatile int t2_done = 0;

static void task1(void) {
    while (!t2);

    do {
        task_switch((task_t *)t2);
    } while (!t2_done);

    t1_done = 1;
}

static void task2(void) {
    t2_done = 1;
}

static int cooperative_task_test(char *message, int len) {
    /* Make the tasks priority 0, so that they will
     * never be preemptively scheduled */
    t1 = new_task(&task1, 0, 0);
    t2 = new_task(&task2, 0, 0);

    int count = 1 << 20;

    /* Continuously switch to task until it finishes */
    do {
        task_switch((task_t *)t1);
    } while (!t1_done && count--);

    if (count <= 0) {
        return FAILED;
    }

    return PASSED;
}
DEFINE_TEST("Cooperative task switching", cooperative_task_test);
