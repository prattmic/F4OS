#ifndef KERNEL_SCHED_SCHED_INTERNALS_H_INCLUDED
#define KERNEL_SCHED_SCHED_INTERNALS_H_INCLUDED

#define STKSIZE     CONFIG_TASK_STACK_SIZE      /* This is in words */

#include <list.h>

struct list runnable_task_list;
struct list periodic_task_list;
struct list free_task_list;

void create_context(task_ctrl *task, void (*lptr)(void)) __attribute__((section(".kernel")));
void _register_task(task_ctrl *task, int periodic) __attribute__((section(".kernel")));

void switch_task(task_ctrl *task) __attribute__((section(".kernel")));
void rtos_tick(void) __attribute__((section(".kernel")));

void end_task(void) __attribute__((section(".kernel"),naked));
void svc_end_task(void) __attribute__((section(".kernel")));
void free_task(task_ctrl *task) __attribute__((section(".kernel")));

uint8_t task_exists(task_ctrl *task) __attribute__((section(".kernel")));

void kernel_task(void) __attribute__((section(".kernel")));
void sleep_task(void) __attribute__((section(".kernel")));

extern void enable_psp(uint32_t *);
extern void disable_psp();
extern uint32_t *save_context(void) __attribute__((section(".kernel"), naked));
extern uint32_t *restore_context() __attribute__((section(".kernel"), naked));
extern uint32_t *restore_full_context() __attribute__((section(".kernel"), naked));

/* Place task in task list based on priority
 * Struct member and global task list have same name */
#define DECLARE_INSERT_TASK_FUNC(task_list_name)                                    \
    void _insert_task_##task_list_name(struct task_ctrl *new_task);                 \

#define DEFINE_INSERT_TASK_FUNC(task_list_name)                                     \
    void _insert_task_##task_list_name(struct task_ctrl *new_task) {                \
        struct list *element;                                                       \
        struct list *insert_point = &task_list_name;                                \
                                                                                    \
        list_for_each(element, &task_list_name) {                                   \
            struct task_ctrl *task = list_entry(element, struct task_ctrl, task_list_name); \
                                                                                    \
            if (task->priority > new_task->priority) {                              \
                break;                                                              \
            }                                                                       \
                                                                                    \
            insert_point = element;                                                 \
        }                                                                           \
                                                                                    \
        list_insert_before(&new_task->task_list_name, insert_point);                \
    }

#define insert_task(task_list_name, new_task)   _insert_task_##task_list_name(new_task)

DECLARE_INSERT_TASK_FUNC(runnable_task_list);
DECLARE_INSERT_TASK_FUNC(periodic_task_list);

#endif
