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

#include <kernel/fault.h>
#include <kernel/mutex.h>
#include <kernel/reentrant_mutex.h>
#include <kernel/sched.h>

void reentrant_acquire(struct reentrant_mutex *mutex) {
    PANIC_ON(!mutex);

    if (mutex->held_by == curr_task) {
        mutex->count++;
    }
    else {
        acquire(&mutex->lock);
        mutex->held_by = curr_task;
        mutex->count = 1;
    }
}

void reentrant_release(struct reentrant_mutex *mutex) {
    PANIC_ON(!mutex);
    PANIC_ON(mutex->held_by != curr_task);

    if (!--mutex->count) {
        mutex->held_by = NULL;
        release(&mutex->lock);
    }

    WARN_ON(mutex->count < 0);
}

