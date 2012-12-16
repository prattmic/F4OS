#ifndef KERNEL_SCHED_H_INCLUDED
#define KERNEL_SCHED_H_INCLUDED

#include <stdint.h>

#define RESOURCE_TABLE_SIZE         16
typedef uint8_t rd_t;
struct resource;
typedef struct resource resource;

#define HELD_SEMAPHORES_MAX         6
struct semaphore;
typedef struct semaphore semaphore;


/* Make a SVC call */

/* I'm so sorry, the optimizer made me do it, I swear!
 * Actually, the compiler refused to inline my always_inline
 * function at -O0, so I had to switch to a macro */
/* We need to make sure that we get the return value
 * without screwing up r0, since GCC doesn't understand that
 * SVC has a return value */
#define SVC(call)  ({ \
    uint32_t ret = 0;   \
    asm volatile ("svc  %[code]  \n"    \
                  "mov  %[ret], r0  \n" \
                  :[ret] "+r" (ret)     \
                  :[code] "I" (call)    \
                  :"r0");               \
    ret;    \
})

#define SVC_ARG(call, arg)  ({ \
    uint32_t ret = 0;   \
    asm volatile ("mov  r0, %[ar]  \n"  \
                  "svc  %[code]  \n"    \
                  "mov  %[ret], r0  \n" \
                  :[ret] "+r" (ret)     \
                  :[code] "I" (call), [ar] "r" (arg)     \
                  :"r0");               \
    ret;    \
})

/* SVC case names */
enum service_calls {
    SVC_YIELD,
    SVC_END_TASK,
    SVC_ACQUIRE,
    SVC_RELEASE
};
   
typedef struct task_node {
    struct task_node        *prev;
    struct task_node        *next;
    struct task_ctrl        *task;
} task_node;

typedef struct task_node_list {
    task_node   *head;
    task_node   *tail;
} task_node_list;

typedef struct task_ctrl {
    uint32_t    *stack_limit;
    uint32_t    *stack_top;
    uint32_t    *stack_base;
    void        (*fptr)(void);
    uint32_t    period;
    uint32_t    ticks_until_wake;
    uint8_t     priority;
    uint8_t     running;
    uint8_t     abort;
    uint32_t    pid;
    task_node   *task_list_node;
    task_node   *periodic_node;
    resource    *resources[RESOURCE_TABLE_SIZE];
    rd_t        top_rd;
    semaphore   *held_semaphores[HELD_SEMAPHORES_MAX];
    semaphore   *waiting;
} task_ctrl;

task_node * volatile curr_task;
extern uint8_t task_switching;

void start_sched(void);
void new_task(void (*fptr)(void), uint8_t priority, uint32_t period) __attribute__((section(".kernel")));
int approx_num_tasks(void);

void raise_privilege(void) __attribute__((section(".kernel")));

/* End-users set up boot tasks here */
void main(void);

#endif
