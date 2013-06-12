#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <dev/registers.h>
#include <kernel/sched.h>
#include <kernel/fault.h>

#include <kernel/semaphore.h>

static void held_semaphores_insert(struct semaphore *list[], volatile struct semaphore *semaphore) __attribute__((section(".kernel")));
void held_semaphores_remove(struct semaphore *list[], volatile struct semaphore *semaphore) __attribute__((section(".kernel")));
static void deadlock_check(struct task_t *task) __attribute__((section(".kernel")));

void acquire(volatile struct semaphore *semaphore) {
    if (!task_switching) {
        semaphore->lock = 1;
        semaphore->held_by = (task_t *) 0x0badf00d;
        return;
    }

    int success = 0;

    while (!success) {
        success = SVC_ARG(SVC_ACQUIRE, (void *) semaphore);
    }
}

/* Acquire semaphore, but remove from held semaphores list so that it can be freed. */
void acquire_for_free(volatile struct semaphore *semaphore) {
    acquire(semaphore);
    held_semaphores_remove(curr_task->semaphore_data.held_semaphores, semaphore);
}

static int8_t try_lock(volatile uint8_t *l) {
    uint8_t taken = 1;
    uint8_t ret;
    uint8_t tmp = 0;
    __asm__("\
        ldrexb      %[tmp], [%[addr]]             \r\n\
        cmp         %[tmp], #0                    \r\n\
        ITT         EQ                            \r\n\
        strexbeq    %[tmp], %[taken], [%[addr]]   \r\n\
        cmpeq       %[tmp], #0                    \r\n\
        ITE         EQ                            \r\n\
        moveq       %[ret], #1                    \r\n\
        movne       %[ret], #0\
        "
        :[ret] "=l" (ret)
        :[addr] "l" (l), [tmp] "l" (tmp), [taken] "l" (taken)
        :"cc", "memory");
    return ret;
}


static int get_lock(volatile struct semaphore *semaphore) {
    struct task_semaphore_data *curr_task_data = &curr_task->semaphore_data;

    if (try_lock(&semaphore->lock)) {
        semaphore->held_by = curr_task;
        held_semaphores_insert(curr_task_data->held_semaphores, semaphore);
        curr_task_data->waiting = NULL;
        return 1;
    }
    else {
        if (semaphore->held_by != NULL) {
            deadlock_check(semaphore->held_by);

            /* Add to waitlist if higher priority */
            if (semaphore->waiting) {
                if (task_compare(semaphore->waiting, curr_task) < 0) {
                    semaphore->waiting = curr_task;
                    curr_task_data->waiting = (struct semaphore *) semaphore;
                }
            }
            else {
                semaphore->waiting = curr_task;
                curr_task_data->waiting = (struct semaphore *) semaphore;
            }

            return 0;
        }
        else {
            panic_print("Semaphore (0x%x) not available, but held_by unset.", semaphore);
        }
    }

    return 0;
}

void release(volatile struct semaphore *semaphore) {
    if (!semaphore->lock) { /* WTF, don't release an unlocked semaphore */
        semaphore->held_by = NULL;
        semaphore->waiting = NULL;
        return;
    }

    if (!task_switching) {
        semaphore->lock = 0;
        semaphore->held_by = NULL;
        return;
    }

    SVC_ARG(SVC_RELEASE, (void *) semaphore);
}

static void held_semaphores_insert(struct semaphore *list[], volatile struct semaphore *semaphore) {
    for (int i = 0; i < HELD_SEMAPHORES_MAX; i++) {
        if (list[i] == NULL) {
            list[i] = (struct semaphore *) semaphore;
            return;
        }
    }

    panic_print("Too many semaphores already held in list (0x%x).", list);
}

void held_semaphores_remove(struct semaphore *list[], volatile struct semaphore *semaphore) {
    for (int i = 0; i < HELD_SEMAPHORES_MAX; i++) {
        if (list[i] == semaphore) {
            list[i] = NULL;
            return;
        }
    }
    /* Not found, but this may be fine, as kernel_task frees semaphores on behalf of the deceased */
}

static void deadlock_check(struct task_t *task) {
    struct task_semaphore_data *task_data = &task->semaphore_data;

    if (task_data->waiting) {
        for (int i = 0; i < HELD_SEMAPHORES_MAX; i++) {
            struct task_semaphore_data *curr_task_data = &curr_task->semaphore_data;
            if (curr_task_data->held_semaphores[i] == task_data->waiting) {
                panic_print("Deadlock!  Task (0x%x) is waiting on semaphore 0x%x, "
                            "but curr_task (0x%x) holds it.", task, task_data->waiting,
                            curr_task);
            }
        }
    }
}

void task_semaphore_setup(task_t *task) {
    struct task_semaphore_data *sem_data = &task->semaphore_data;

    memset(sem_data->held_semaphores, 0, sizeof(sem_data->held_semaphores));
    sem_data->waiting = NULL;
}

static int svc_acquire(struct semaphore *semaphore) {
    int ret;

    if (get_lock(semaphore)) {
        /* Success */
        ret = 1;
    }
    else {
        /* Failure */
        ret = 0;

        if (task_runnable(semaphore->held_by)
                && (task_compare(semaphore->held_by, curr_task) <= 0)) {
            task_switch(semaphore->held_by);
        }
        else {
            /* If task was not runnable, it was either a period task
             * between runs, or it recently ended without releasing
             * the semaphore.  In that case, the kernel task will
             * release the semaphore on its next run. */
            task_switch(NULL);
        }
    }

    return ret;
}

static void svc_release(struct semaphore *semaphore) {
    semaphore->lock = 0;
    semaphore->held_by = NULL;
    held_semaphores_remove(curr_task->semaphore_data.held_semaphores, semaphore);

    if (semaphore->waiting
            && (task_compare(semaphore->waiting, curr_task) >= 0)) {
        task_t *task = semaphore->waiting;
        semaphore->waiting = NULL;

        task_switch(task);
    }
}

void svc_semaphore(uint32_t svc_number, uint32_t *registers) {
    if (!IPSR()) {
        panic_print("Attempted to call service call from user space");
    }

    switch (svc_number) {
        case SVC_ACQUIRE:
            registers[0] = svc_acquire((struct semaphore *) registers[0]);
            break;
        case SVC_RELEASE:
            svc_release((struct semaphore *) registers[0]);
            break;
        default:
            panic_print("Unknown SVC: %d", svc_number);
            break;
    }
}
