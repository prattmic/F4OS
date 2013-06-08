#ifndef KERNEL_SCHED_H_INCLUDED
#define KERNEL_SCHED_H_INCLUDED

#include <list.h>
#include <stdint.h>
#include <dev/cortex_m.h>

#define RESOURCE_TABLE_SIZE         CONFIG_RESOURCE_TABLE_SIZE
typedef int8_t rd_t;
struct resource;
typedef struct resource resource;

#define HELD_SEMAPHORES_MAX         CONFIG_HELD_SEMAPHORES_MAX
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

#define SVC_ARG2(call, arg1, arg2)  ({ \
    uint32_t ret = 0;   \
    asm volatile ("mov  r0, %[ar1]  \n"  \
                  "mov  r1, %[ar2]  \n"  \
                  "svc  %[code]  \n"    \
                  "mov  %[ret], r0  \n" \
                  :[ret] "+r" (ret)     \
                  :[code] "I" (call), [ar1] "r" (arg1), [ar2] "r" (arg2)     \
                  :"r0", "r1");               \
    ret;    \
})

/* SVC case names */
enum service_calls {
    SVC_YIELD,
    SVC_END_TASK,
    SVC_ACQUIRE,
    SVC_RELEASE,
    SVC_REGISTER_TASK,
};

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
    resource    *resources[RESOURCE_TABLE_SIZE];
    rd_t        top_rd;
    semaphore   *held_semaphores[HELD_SEMAPHORES_MAX];
    semaphore   *waiting;
    struct list runnable_task_list;
    struct list periodic_task_list;
    struct list free_task_list;
} task_ctrl;

task_ctrl * volatile curr_task;
extern uint8_t task_switching;
extern volatile uint32_t total_tasks;

void start_sched(void);
void new_task(void (*fptr)(void), uint8_t priority, uint32_t period) __attribute__((section(".kernel")));

void raise_privilege(void) __attribute__((section(".kernel")));

/* End-users set up boot tasks here */
void main(void);

/* Only perform a yield if task switching is active, and we are
 * not in an interrupt context */
static inline yield_if_possible(void) __attribute__((always_inline));
static inline yield_if_possible(void) {
    if (task_switching && !IPSR()) {
        SVC(SVC_YIELD);
    }
}

#endif
