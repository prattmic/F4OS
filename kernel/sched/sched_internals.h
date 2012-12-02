#ifndef KERNEL_SCHED_SCHED_INTERNALS_H_INCLUDED
#define KERNEL_SCHED_SCHED_INTERNALS_H_INCLUDED

#define STKSIZE                 255                      /* This is in words */

task_node_list task_list;
task_node_list periodic_task_list;

extern task_node * volatile task_to_free;

void append_task(task_node_list *list, task_node *task) __attribute__((section(".kernel")));
void create_context(task_ctrl *task, void (*lptr)(void)) __attribute__((section(".kernel")));

void switch_task(task_node *node) __attribute__((section(".kernel")));
void rtos_tick(void) __attribute__((section(".kernel")));

void remove_task(task_node_list *list, task_node *tasknode) __attribute__((section(".kernel")));
void free_task(task_node *tasknode) __attribute__((section(".kernel")));
void end_task(void) __attribute__((section(".kernel"),naked));
void svc_end_task(void) __attribute__((section(".kernel")));

void kernel_task(void) __attribute__((section(".kernel")));

extern void enable_psp(uint32_t *);
extern void disable_psp();
extern uint32_t *save_context(void) __attribute__((section(".kernel"), naked));
extern uint32_t *restore_context() __attribute__((section(".kernel"), naked));
extern uint32_t *restore_full_context() __attribute__((section(".kernel"), naked));

#endif
