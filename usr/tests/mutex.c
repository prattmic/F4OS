/*
 * Copyright (C) 2014 F4OS Authors
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
#include <time.h>
#include <kernel/reentrant_mutex.h>
#include "test.h"

int reentrant_mutex_basic_test(char *message, int len) {
    struct reentrant_mutex mutex = INIT_REENTRANT_MUTEX;

    reentrant_acquire(&mutex);
    reentrant_acquire(&mutex);
    reentrant_release(&mutex);
    reentrant_release(&mutex);

    reentrant_acquire(&mutex);
    reentrant_acquire(&mutex);
    reentrant_release(&mutex);
    reentrant_release(&mutex);

    return PASSED;
}
DEFINE_TEST("Reentrant mutex basic", reentrant_mutex_basic_test);

struct reentrant_mutex task_test_lock = INIT_REENTRANT_MUTEX;
volatile int success = 0;

void acquire_task(void) {
    reentrant_acquire(&task_test_lock);
    reentrant_release(&task_test_lock);

    success = 1;
}

int reentrant_mutex_task_test(char *message, int len) {
    reentrant_acquire(&task_test_lock);
    reentrant_acquire(&task_test_lock);

    new_task(&acquire_task, 1, 0);
    usleep(10000);

    reentrant_release(&task_test_lock);
    reentrant_release(&task_test_lock);

    while (!success);

    return PASSED;
}
DEFINE_TEST("Reentrant mutex task", reentrant_mutex_task_test);
