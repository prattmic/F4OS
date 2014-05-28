/*
 * Copyright (C) 2013, 2014 F4OS Authors
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

#ifndef KERNEL_MUTEX_H_INCLUDED
#define KERNEL_MUTEX_H_INCLUDED

#include <stdint.h>
#include <stddef.h>

#define HELD_MUTEXES_MAX         CONFIG_HELD_MUTEXES_MAX

struct task_t;
typedef struct task_t task_t;

struct mutex {
        uint8_t lock;
        task_t  *held_by;
        task_t  *waiting;
};

typedef struct mutex mutex;

struct task_mutex_data {
    struct mutex   *held_mutexes[HELD_MUTEXES_MAX];
    struct mutex   *waiting;
};

void acquire(volatile struct mutex *mutex);
void acquire_for_free(volatile struct mutex *mutex);
void release(volatile struct mutex *mutex);

static inline void init_mutex(volatile struct mutex *mutex) {
    mutex->lock = 0;
    mutex->held_by = NULL;
    mutex->waiting = NULL;
}

#define INIT_MUTEX  {\
    .lock = 0,          \
    .held_by = NULL,    \
    .waiting = NULL,    \
}

/* Setup mutex data structure for a new task */
void task_mutex_setup(task_t *task);

/**
 * Semaphore service call handler
 * Should only be called by global SVC handler.  This takes va_args for the
 * mutex service calls and returns the result of the service call.
 *
 * @param svc_number    Service call number.  Must be a mutex service call
 * @param va_args       Arguments for service call
 * @returns Return value of service call
 */
int mutex_service_call(uint32_t svc_number, ...);

#endif
