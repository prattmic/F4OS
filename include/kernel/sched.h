#ifndef KERNEL_SCHED_H_INCLUDED
#define KERNEL_SCHED_H_INCLUDED

#include <stdint.h>

#define RESOURCE_TABLE_SIZE         16
typedef uint8_t rd_t;

/* Make a SVC call */
#define _svc(x)                         asm volatile ("svc  %0  \n" :: "i" (x))

/* SVC case names */
#define     SVC_RAISE_PRIV      0x0
#define     SVC_YIELD           0x1
#define     SVC_END_TASK        0x2
#define     SVC_END_PERIODIC_TASK   0x3
   
typedef struct task_node {
    struct task_node        *prev;
    struct task_node        *next;
    struct task_ctrl        *task;
} task_node;

typedef struct task_node_list {
    task_node   *head;
    task_node   *tail;
} task_node_list;

struct resource;
typedef struct resource resource;

typedef struct task_ctrl {
    uint32_t    *stack_limit;
    uint32_t    *stack_top;
    uint32_t    *stack_base;
    void        (*fptr)(void);
    uint32_t    period;
    uint32_t    ticks_until_wake;
    uint8_t     priority;
    uint8_t     running;
    uint32_t    pid;
    task_node   *task_list_node;
    task_node   *periodic_node;
    resource    *resources[RESOURCE_TABLE_SIZE];
    rd_t        top_rd;
} task_ctrl;

task_node * volatile curr_task;
extern uint8_t task_switching;

void start_sched(void);
void new_task(void (*fptr)(void), uint8_t priority, uint32_t period) __attribute__((section(".kernel")));
void swap_task(task_node *node) __attribute__((section(".kernel"), optimize(0)));

void raise_privilege(void) __attribute__((section(".kernel")));

/* End-users set up boot tasks here */
void main(void);

#endif
