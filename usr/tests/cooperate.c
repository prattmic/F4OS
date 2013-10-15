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
