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

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <kernel/sched.h>
#include <kernel/fault.h>

#include <kernel/mutex.h>

static void held_mutexes_insert(struct mutex *list[], volatile struct mutex *mutex) __attribute__((section(".kernel")));
void held_mutexes_remove(struct mutex *list[], volatile struct mutex *mutex) __attribute__((section(".kernel")));
static void deadlock_check(volatile struct mutex *mut) __attribute__((section(".kernel")));

void acquire(volatile struct mutex *mutex) {
    if (!task_switching) {
        mutex->lock = 1;
        mutex->held_by = (task_t *) 0x0badf00d;
        return;
    }

    int success = 0;

    while (!success) {
        success = SVC_ARG(SVC_ACQUIRE, (void *) mutex);
    }
}

/* Acquire mutex, but remove from held mutexes list so that it can be freed. */
void acquire_for_free(volatile struct mutex *mutex) {
    acquire(mutex);
    held_mutexes_remove(curr_task->mutex_data.held_mutexes, mutex);
}

static int get_lock(volatile struct mutex *mutex) {
    struct task_mutex_data *curr_task_data = &curr_task->mutex_data;

    /* TODO: Use a nicer name than this builtin */
    if (__sync_bool_compare_and_swap(&mutex->lock, 0, 1)) {
        mutex->held_by = curr_task;
        held_mutexes_insert(curr_task_data->held_mutexes, mutex);
        curr_task_data->waiting = NULL;
        return 1;
    }
    else {
        if (mutex->held_by != NULL) {
            deadlock_check(mutex);

            /* Add to waitlist if higher priority */
            if (mutex->waiting) {
                if (task_compare(mutex->waiting, curr_task) < 0) {
                    mutex->waiting = curr_task;
                    curr_task_data->waiting = (struct mutex *) mutex;
                }
            }
            else {
                mutex->waiting = curr_task;
                curr_task_data->waiting = (struct mutex *) mutex;
            }

            return 0;
        }
        else {
            panic_print("Semaphore (0x%x) not available, but held_by unset.", mutex);
        }
    }

    return 0;
}

void release(volatile struct mutex *mutex) {
    if (!mutex->lock) { /* WTF, don't release an unlocked mutex */
        mutex->held_by = NULL;
        mutex->waiting = NULL;
        return;
    }

    if (!task_switching) {
        mutex->lock = 0;
        mutex->held_by = NULL;
        return;
    }

    SVC_ARG(SVC_RELEASE, (void *) mutex);
}

static void held_mutexes_insert(struct mutex *list[], volatile struct mutex *mutex) {
    for (int i = 0; i < HELD_MUTEXES_MAX; i++) {
        if (list[i] == NULL) {
            list[i] = (struct mutex *) mutex;
            return;
        }
    }

    panic_print("Too many mutexes already held in list (0x%x).", list);
}

void held_mutexes_remove(struct mutex *list[], volatile struct mutex *mutex) {
    for (int i = 0; i < HELD_MUTEXES_MAX; i++) {
        if (list[i] == mutex) {
            list[i] = NULL;
            return;
        }
    }
    /* Not found, but this may be fine, as kernel_task frees mutexes on behalf of the deceased */
}

static void deadlock_check(volatile struct mutex *mut) {
    struct task_t *task = mut->held_by;
    struct task_mutex_data *task_data = &task->mutex_data;

    if (task == curr_task) {
        panic_print("Task (0x%x) attempted to double acquire mutex 0x%x",
                    curr_task, mut);
    }

    if (task_data->waiting) {
        for (int i = 0; i < HELD_MUTEXES_MAX; i++) {
            struct task_mutex_data *curr_task_data = &curr_task->mutex_data;
            if (curr_task_data->held_mutexes[i] == task_data->waiting) {
                panic_print("Deadlock!  Task (0x%x) is waiting on mutex 0x%x, "
                            "but curr_task (0x%x) holds it.", task, task_data->waiting,
                            curr_task);
            }
        }
    }
}

void task_mutex_setup(task_t *task) {
    struct task_mutex_data *mut_data = &task->mutex_data;

    memset(mut_data->held_mutexes, 0, sizeof(mut_data->held_mutexes));
    mut_data->waiting = NULL;
}

static int svc_acquire(struct mutex *mutex) {
    int ret;

    if (get_lock(mutex)) {
        /* Success */
        ret = 1;
    }
    else {
        /* Failure */
        ret = 0;

        if (task_runnable(mutex->held_by)
                && (task_compare(mutex->held_by, curr_task) <= 0)) {
            task_switch(mutex->held_by);
        }
        else {
            /* If task was not runnable, it was either a period task
             * between runs, or it recently ended without releasing
             * the mutex.  In that case, the kernel task will
             * release the mutex on its next run. */
            task_switch(NULL);
        }
    }

    return ret;
}

static void svc_release(struct mutex *mutex) {
    mutex->lock = 0;
    mutex->held_by = NULL;
    held_mutexes_remove(curr_task->mutex_data.held_mutexes, mutex);

    if (mutex->waiting
            && (task_compare(mutex->waiting, curr_task) >= 0)) {
        task_t *task = mutex->waiting;
        mutex->waiting = NULL;

        task_switch(task);
    }
}

int mutex_service_call(uint32_t svc_number, ...) {
    int ret = 0;
    va_list ap;
    va_start(ap, svc_number);

    switch (svc_number) {
        case SVC_ACQUIRE: {
            struct mutex *mut = va_arg(ap, struct mutex *);
            ret = svc_acquire(mut);
            break;
        }
        case SVC_RELEASE: {
            struct mutex *mut = va_arg(ap, struct mutex *);
            svc_release(mut);
            break;
        }
        default:
            panic_print("Unknown SVC: %d", svc_number);
            break;
    }

    va_end(ap);

    return ret;
}
