#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <mm/mm.h>
#include <dev/resource.h>
#include <kernel/fault.h>

#include <kernel/sched.h>
#include "sched_internals.h"

static task_ctrl *create_task(void (*fptr)(void), uint8_t priority, uint32_t period) __attribute__((section(".kernel")));
static task_node *register_task(task_node_list *list, task_ctrl *task_ptr) __attribute__((section(".kernel")));

void new_task(void (*fptr)(void), uint8_t priority, uint32_t period) {
    task_ctrl *task = create_task(fptr, priority, period);
    if (task != NULL) {
        task_node *reg_task;

        reg_task = register_task(&task_list, task);
        if (reg_task == NULL) {
            free(task->stack_limit);
            kfree(task);
            printf("Could not allocate task with function pointer 0x%x; panicking.\r\n", fptr);
            panic_print("Could not allocate task.");
        }
        else {
            task->task_list_node = reg_task;
        }

        if (period) {
            task_node *per_node = register_task(&periodic_task_list, task);
            if (per_node == NULL) {
                free(task->stack_limit);
                kfree(task);
                kfree(reg_task);
                printf("Could not allocate task with function pointer 0x%x; panicking.\r\n", fptr);
                panic_print("Could not allocate task.");
            }
            else {
                task->periodic_node = per_node;
            }
        }
    }
    else {
        printf("Could not allocate task with function pointer 0x%x; panicking.\r\n", fptr);
        panic_print("Could not allocate task.");
    }
}

/* Place task in task list based on priority */
void append_task(task_node_list *list, task_node *task) {
    /* Interrupts need to be disabled while modifying the task
     * list, as an interrupt could find the list chopped in two */
    __asm__("cpsid  i");

    /* Check if head is set */
    if (list->head == NULL) {
        if (list->tail) {
            /* WTF!?  Why is the tail set but not the head? */
            panic_print("Task list tail set, but not head.");
        }

        list->head = task;
        list->tail = task;
        task->prev = NULL;
        task->next = NULL;
    }
    else {
        if (task->task->priority > list->head->task->priority) {
            task->prev = NULL;
            task->next = list->head;
            task->next->prev = task;
            list->head = task;

            __asm__("cpsie  i");
            return;
        }
        else {
            task_node *prev = list->head;
            task_node *next = list->head->next;

            while (next && next->task->priority >= task->task->priority) {
                prev = next;
                next = next->next;
            }
            
            if (next) {
                task->next = next;
                next->prev = task;
            }
            else {
                task->next = NULL;
                list->tail = task;
            }
            task->prev = prev;
            prev->next = task;
        }
    }

    __asm__("cpsie  i");
}

void create_context(task_ctrl* task, void (*lptr)(void)) {
    asm volatile("mov     r5, #0                                                              \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  ldr.w   r5, =0x01000000 /* Thumb state bit must be set */                   \n\
                  stmdb   %[stack]!, {r5}   /* xPSR */                                        \n\
                  mov     r5, #0                                                              \n\
                  stmdb   %[stack]!, {%[pc]}   /* PC */                                       \n\
                  stmdb   %[stack]!, {%[lr]}   /* LR */                                       \n\
                  stmdb   %[stack]!, {r5}   /* R12 */                                         \n\
                  stmdb   %[stack]!, {r5}   /* R3 */                                          \n\
                  stmdb   %[stack]!, {r5}   /* R2 */                                          \n\
                  stmdb   %[stack]!, {r5}   /* R1 */                                          \n\
                  stmdb   %[stack]!, {r5}   /* R0 */                                          \n\
                  stmdb   %[stack]!, {r5}   /* R11 */                                         \n\
                  stmdb   %[stack]!, {r5}   /* R10 */                                         \n\
                  stmdb   %[stack]!, {r5}   /* R9 */                                          \n\
                  stmdb   %[stack]!, {r5}   /* R8 */                                          \n\
                  stmdb   %[stack]!, {%[frame]}   /* R7 - Frame Pointer*/                     \n\
                  stmdb   %[stack]!, {r5}   /* R6 */                                          \n\
                  stmdb   %[stack]!, {r5}   /* R5 */                                          \n\
                  stmdb   %[stack]!, {r5}   /* R4 */"
                  :[stack] "+r" (task->stack_top) /* Output */
                  :[pc] "r" (task->fptr), [lr] "r" (lptr), [frame] "r" (task->stack_limit)   /* Input */
                  :"r5"   /* Clobber */);

}

static task_ctrl *create_task(void (*fptr)(void), uint8_t priority, uint32_t period) {
    task_ctrl *task;
    uint32_t *memory;
    static uint32_t pid_source = 1;
    task = (task_ctrl *) kmalloc(sizeof(task_ctrl));
    if (task == NULL) {
        return NULL;
    }
    
    memory = (uint32_t *) malloc(STKSIZE*4);
    if (memory == NULL) {
        kfree(task);
        return NULL;
    }

    task->stack_limit       = memory;
    task->stack_base        = memory + STKSIZE;
    task->stack_top         = memory + STKSIZE;
    task->fptr              = fptr;
    task->priority          = priority;
    task->running           = 0;

    task->period            = period;
    task->ticks_until_wake  = period;

    task->task_list_node    = NULL;
    task->periodic_node     = NULL;
    task->pid               = pid_source++;

    resource_setup(task);
    return task;
}

static task_node *register_task(task_node_list *list, task_ctrl *task_ptr) {
    task_node *new_task = kmalloc(sizeof(task_node));
    if (new_task == NULL) {
        return NULL;
    }

    new_task->task = task_ptr;
    append_task(list, new_task);

    return new_task;
}
