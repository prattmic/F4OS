#ifndef KERNEL_SCHED_SCHED_INTERNALS_H_INCLUDED
#define KERNEL_SCHED_SCHED_INTERNALS_H_INCLUDED

#include <stdint.h>
#include <list.h>

#define STKSIZE     CONFIG_TASK_STACK_SIZE      /* This is in words */

struct list runnable_task_list;
struct list periodic_task_list;
struct list free_task_list;

void svc_register_task(task_ctrl *task, int periodic) __attribute__((section(".kernel")));

int svc_task_switch(task_ctrl *task) __attribute__((section(".kernel")));

void end_task(void) __attribute__((section(".kernel"),naked));
void sched_svc_end_task(void) __attribute__((section(".kernel")));
void free_task(task_ctrl *task) __attribute__((section(".kernel")));

uint8_t task_exists(task_t *task) __attribute__((section(".kernel")));

void kernel_task(void) __attribute__((section(".kernel")));
void sleep_task(void) __attribute__((section(".kernel")));

/* Place task in task list based on priority
 * Struct member and global task list have same name */
#define DECLARE_INSERT_TASK_FUNC(task_list_name)                                    \
    void _insert_task_##task_list_name(struct task_ctrl *new_task);                 \

#define DEFINE_INSERT_TASK_FUNC(task_list_name)                                     \
    void _insert_task_##task_list_name(struct task_ctrl *new_task) {                \
        /* First task in list */                                                    \
        struct task_ctrl *task = list_entry(task_list_name.next,                    \
                struct task_ctrl, task_list_name);                                  \
                                                                                    \
        /* New task is the highest priority, add to front */                        \
        if (list_empty(&task_list_name) || (new_task->priority > task->priority)) { \
            list_add_head(&new_task->task_list_name, &task_list_name);              \
            return;                                                                 \
        }                                                                           \
                                                                                    \
        /* Last task in list */                                                     \
        task = list_entry(list_tail(&task_list_name), struct task_ctrl,             \
                task_list_name);                                                    \
                                                                                    \
        /* New task is the lowest priority, add to end */                           \
        if (new_task->priority <= task->priority) {                                 \
            list_add_tail(&new_task->task_list_name, &task_list_name);              \
            return;                                                                 \
        }                                                                           \
                                                                                    \
        /* New task is in the middle, add after last equal priority task */         \
        struct list *element;                                                       \
        struct list *insert_point = NULL;                                           \
                                                                                    \
        list_for_each(element, &task_list_name) {                                   \
            struct task_ctrl *task = list_entry(element, struct task_ctrl,          \
                    task_list_name);                                                \
                                                                                    \
            if (new_task->priority > task->priority) {                              \
                insert_point = element;                                             \
            }                                                                       \
        }                                                                           \
                                                                                    \
        if (!insert_point) {                                                        \
            panic_print("Unable to place priority %d task in %s",                   \
                    new_task->priority, #task_list_name);                           \
        }                                                                           \
                                                                                    \
        list_insert_before(&new_task->task_list_name, insert_point);                \
    }

#define insert_task(task_list_name, new_task)   _insert_task_##task_list_name(new_task)

DECLARE_INSERT_TASK_FUNC(runnable_task_list);
DECLARE_INSERT_TASK_FUNC(periodic_task_list);

#endif
