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

#ifndef KERNEL_REENTRANT_MUTEX_H_INCLUDED
#define KERNEL_REENTRANT_MUTEX_H_INCLUDED

#include <stddef.h>
#include <kernel/mutex.h>
#include <kernel/sched.h>

struct reentrant_mutex {
    struct mutex lock;
    struct task_t *held_by;
    int count;
};

/*
 * Statically initialize reentrant mutex
 *
 * At compile-time, statically initialize a struct reentrant_mutex
 * for use.
 *
 * struct reentrant_mutex mutex = INIT_REENTRANT_MUTEX;
 */
#define INIT_REENTRANT_MUTEX    {   \
    .lock = INIT_MUTEX,             \
    .held_by = NULL,                \
    .count = 0,                     \
}

/*
 * Dynamically initialize reentrant mutex
 *
 * At runtime, initialize a struct reentrant_mutex for use.
 *
 * @param mutex mutex to initialize
 */
static inline void init_reentrant_mutex(struct reentrant_mutex *mutex) {
    init_mutex(&mutex->lock);
    mutex->held_by = NULL;
    mutex->count = 0;
}

/*
 * Acquire reentrant mutex
 *
 * Returns immediately if lock is already held, otherwise blocks until
 * lock can be acquired.
 *
 * reentrant_release() must be called when finished with lock, as many
 * times as reentrant_acquire() was called.
 *
 * @param mutex lock to acquire
 */
void reentrant_acquire(struct reentrant_mutex *mutex);

/*
 * Release reentrant mutex
 *
 * Releases innermost acquire of mutex.  When called as many times as
 * reentrant_acquire(), actually releases mutex to the OS.
 *
 * @param mutex lock to release
 */
void reentrant_release(struct reentrant_mutex *mutex);

#endif
