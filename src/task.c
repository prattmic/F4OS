#include "types.h"
#include "registers.h"
#include "interrupt.h"
#include "usart.h"
#include "task.h"
#include "context.h"
#include "semaphore.h"
#include "buddy.h"
#include "stdio.h"
#include "mem.h"
#include "resource.h"

task_node * volatile task_to_free = NULL;
uint8_t task_switching = 0;

void kernel_task(void) {
    /* Does cleanup that can't be done from outside a task (ie. in an interrupt) */

    while (1) {
        while (task_to_free != NULL) {
            task_node *node = task_to_free;

            task_to_free = task_to_free->next;

            free_task(node);
        }

        _svc(SVC_YIELD);
    }
}

void start_task_switching(void) {
    task_ctrl *task = task_list.head->task;

    curr_task = task_list.head;

    //mpu_stack_set(task->stack_base);

    task_switching = 1;
    task->running = 1;
    
    if (task->period) {
        create_context(task, &end_periodic_task);
    }
    else {
        create_context(task, &end_task);
    }

    enable_psp(task->stack_top);
    restore_full_context();
    __asm__("nop");
}

void switch_task(void) {
    /* Rate monotonic scheduling
     * Always runs the highest priority task,
     * which will be the head of the list, as
     * it is kept sorted.  Round-robin through
     * equal priority tasks. */

    /* Temporary fix to ensure tim2_handler doesn't modify the task_list */
    __asm__("cpsid  i");

    task_node *node = task_list.head;
    curr_task = node;
    if (curr_task == NULL) {
        /* Uh-oh, no tasks! */
        __asm__("cpsie  i");
        panic_print("No tasks to run.");
    }

    /* As a workaround for lack of MPU support, check if the 
     * stack of the task we are switching from has overflowed */
    if (node->task->stack_base > node->task->stack_top) {
        __asm__("cpsie  i");
        panic_print("Task has overflowed its stack.");
    }

    /* Don't bother moving this if it is the only high priority task */
    if (node->task->priority <= node->next->task->priority) {
        /* Move to end of priority */
        task_node *prev = node;
        task_node *next = node->next;
        while (next && node->task->priority == next->task->priority) {
            prev = next;
            next = next->next;
        }

        /* Found next priority level */
        if (next) {
            task_list.head = node->next;
            node->next->prev = NULL;

            prev->next = node;
            node->prev = prev;

            next->prev = node;
            node->next = next;
        } 
        /* Reached end of list without priority changing, place on end. */
        else {
            if (node->next) {
                task_list.head = node->next;
                node->next->prev = NULL;
            }

            node->next = NULL;
            node->prev = task_list.tail;
            task_list.tail->next = node;
            task_list.tail = node;
        }
    }

    __asm__("cpsie  i");

    /* mpu_stack_set(node->task->stack_base);   Sigh...maybe some day */

    if (node->task->running) {
        uint32_t *psp_addr = node->task->stack_top;
        enable_psp(psp_addr);
        return;
    }
    else {
        node->task->running = 1;

        if (node->task->period) {
            create_context(node->task, &end_periodic_task);
        }
        else {
            create_context(node->task, &end_task);
        }
        enable_psp(node->task->stack_top);
        return;
    }
}

task_ctrl *create_task(void (*fptr)(void), uint8_t priority, uint32_t period) {
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
    task->stack_base = memory;
    task->stack_top  = memory + STKSIZE;
    task->fptr       = fptr;
    task->priority   = priority;
    task->period     = period;
    task->running    = 0;
    task->stack_base        = memory;
    task->stack_top         = memory + STKSIZE;
    task->fptr              = fptr;
    task->priority          = priority;
    task->running           = 0;

    task->period            = period;
    task->ticks_until_wake  = period;

    task->task_list_node    = NULL;
    task->periodic_node     = NULL;
    task->pid = pid_source++;

    resource_setup(task);
    return task;
}

task_node *register_task(task_node_list *list, task_ctrl *task_ptr) {
    task_node *new_task = kmalloc(sizeof(task_node));
    if (new_task == NULL) {
        return NULL;
    }

    new_task->task = task_ptr;
    append_task(list, new_task);

    return new_task;
}

void new_task(void (*fptr)(void), uint8_t priority, uint32_t period) {
    task_ctrl *task = create_task(fptr, priority, period);
    if (task != NULL) {
        task_node *reg_task;

        reg_task = register_task(&task_list, task);
        if (reg_task == NULL) {
            free(task->stack_base);
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
                free(task->stack_base);
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

void remove_task(task_node_list *list, task_node *node) {
    /* Remove from end/middle/beginning of list */
    if (node->next == NULL && node->prev) {
        node->prev->next = NULL;
    }
    if (node->prev) {
        node->prev->next = node->next;
    }
    else {
        node->next->prev = NULL;
    }

    /* Remove from head/tail of list->*/
    if (list->head == node) {
        list->head = node->next;
    }
    if (list->tail == node) {
        list->tail = node->prev;
    }
}

void idle_task(void) {
    while(1){
        __asm__("nop");
    }
}

void free_task(task_node *node) {
    /* Free memory */
    free(node->task->stack_base);
    kfree(node->task);
    kfree(node);
}

void end_task(void) {
    __asm__("mrs    %[ghetto], msp"
            :[ghetto] "=r" (ghetto_sp_save)::);
    _svc(SVC_END_TASK);    /* Shouldn't return (to here, at least) */
}

void end_periodic_task(void) {
    __asm__("mrs    %[ghetto], msp"
            :[ghetto] "=r" (ghetto_sp_save)::);
    _svc(SVC_END_PERIODIC_TASK);    /* Shouldn't return (to here, at least) */
}
