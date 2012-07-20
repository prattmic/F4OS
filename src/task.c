#include "types.h"
#include "registers.h"
#include "context.h"
#include "buddy.h"
#include "interrupt.h"
#include "usart.h"
#include "task.h"
#include "mem.h"

task_ctrl k_idle_task;

task_node sys_idle_task;
task_node_list task_list;

task_node * volatile task_to_free = NULL;
uint8_t task_switching = 0;

void init_kernel(void) {
    sys_idle_task.next = NULL;
    (sys_idle_task.task)->fptr =        &idle_task;
    (sys_idle_task.task)->stack_base =  IDLE_TASK_BASE;
    (sys_idle_task.task)->stack_top =   IDLE_TASK_BASE;
}

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

    k_curr_task = task_list.head;

    //mpu_stack_set(task->stack_base);

    task_switching = 1;
    task->running = 1;
    
    create_context(task, &end_task);

    enable_psp(task->stack_top);
    restore_full_context();
}

void switch_task(void) {
    /* Rate monotonic scheduling
     * Always runs the highest priority task,
     * which will be the head of the list, as
     * it is kept sorted.  Round-robin through
     * equal priority tasks. */
    task_node *node = task_list.head;
    k_curr_task = node;
    if (k_curr_task == NULL) {
        /* Uh-oh, no tasks! */
        panic();
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
        /* Reached end of list without priorty changing, place on end. */
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

    /* mpu_stack_set(node->task->stack_base);   Sigh...maybe some day */

    if (node->task->running) {
        uint32_t *psp_addr = node->task->stack_top;
        enable_psp(psp_addr);
        return;
    }
    else {
        node->task->running = 1;

        create_context(node->task, &end_task);
        enable_psp(node->task->stack_top);
        return;
    }
}

task_ctrl *create_task(void (*fptr)(void), uint8_t priority, uint32_t period) {
    task_ctrl *task;
    uint32_t *memory;

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

    return task;
}

/* Place task in task list based on priority */
void append_task_to_klist(task_node *new_task) {
    /* Check if head is set */
    if (task_list.head == NULL) {
        if (task_list.tail) {
            /* WTF!?  Why is the tail set but not the head? */
            panic();
        }

        task_list.head = new_task;
        task_list.tail = new_task;
        new_task->prev = NULL;
        new_task->next = NULL;
    }
    else {
        if (new_task->task->priority > task_list.head->task->priority) {
            new_task->prev = NULL;
            new_task->next = task_list.head;
            task_list.head = new_task;
            return;
        }
        else {
            task_node *prev = task_list.head;
            task_node *next = task_list.head->next;

            while (next && next->task->priority >= new_task->task->priority) {
                prev = next;
                next = next->next;
            }
            
            if (next) {
                new_task->next = next;
                next->prev = new_task;
            }
            else {
                new_task->next = NULL;
                task_list.tail = new_task;
            }
            new_task->prev = prev;
            prev->next = new_task;
        }
    }
}

void idle_task(void) {
    while(1){
        __asm__("nop");
    }
}

void end_task(void) {
    __asm__("push {lr}");
    _svc(SVC_END_TASK);    /* Shouldn't return (to here, at least) */
    __asm__("pop {lr}\n"
            "bx lr\n");
}

task_node *register_task(task_ctrl *task_ptr) {
    task_node *new_task = kmalloc(sizeof(task_node));
    if (new_task == NULL) {
        return NULL;
    }

    new_task->task = task_ptr;
    append_task_to_klist(new_task);

    return new_task;
}

void remove_task(task_node *node) {
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

    /* Remove from head/tail of task_list */
    if (task_list.head == node) {
        task_list.head = node->next;
    }
    if (task_list.tail == node) {
        task_list.tail = node->prev;
    }
}

void free_task(task_node *node) {
    /* Free memory */
    free(node->task->stack_base);
    kfree(node->task);
    kfree(node);
}

task_node_list sort_by_priority(task_node_list list) {
    task_node_list ret = {NULL, NULL};
    task_node *temp_head = list.head;
    task_node *ptr = list.head;
    task_node *preptr = list.head;
    task_node *pre_min_node = list.head;
    int8_t min = ptr->task->priority;

    while(ptr->next != NULL){
        if(min >= ptr->task->priority){
            min = ptr->task->priority;
            pre_min_node = preptr;
        }
        preptr = ptr;
        ptr = ptr->next;
        //derp
    }

    ret.head = pre_min_node->next;
    ret.tail = ret.head;
    while(temp_head->next != NULL){
        preptr = ptr;
        ptr = ptr->next;
        while(ptr->next != NULL){
            if(min >= ptr->task->priority){
                min = ptr->task->priority;
                pre_min_node = preptr;
            }
        }
        ret.tail->next = pre_min_node->next;
        ret.tail = ret.tail->next;
    }
    return ret;
}
