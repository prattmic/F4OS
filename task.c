#include "types.h"
#include "registers.h"
#include "mem.h"
#include "context.h"
#include "task.h"
#include "heap.h"

void panic(void);

taskCtrl k_idle_task;

taskNode sys_idle_task;
taskNodeList task_list;
taskNodeList task_queue;

void init_kernel(void){
    sys_idle_task.next_node = NULL;
    (sys_idle_task.task)->fptr =        &idle_task;
    (sys_idle_task.task)->stack_base =  IDLE_TASK_BASE;
    (sys_idle_task.task)->stack_top =   IDLE_TASK_BASE;
}

void start_task_switching(void) {
    taskCtrl *task = task_list.head->task;

    k_currentTask = task_list.head;

    //mpu_stack_set(task->stack_base);
    enable_psp(task->stack_top);

    task->running = 1;
    user_mode_branch(task->fptr);
}

void switch_task(void) {
    taskNode *tasknode = k_currentTask;

    while (tasknode == k_currentTask) {
        if (tasknode->next_node == NULL) {
            if (tasknode == task_list.head) {
                break;
            }
            tasknode = task_list.head;
        }
        else {
            tasknode = tasknode->next_node;
        }
    }

    k_currentTask = tasknode;
    if (k_currentTask == NULL) {
        /* Uh-oh, no tasks! */
        panic();
    }

    //mpu_stack_set(tasknode->task->stack_base);

    if (tasknode->task->running) {
        uint32_t *psp_addr = tasknode->task->stack_top;
        enable_psp(psp_addr);
        return;
    }
    else {
        enable_psp(tasknode->task->stack_top);
        tasknode->task->running = 1;
        /*user_mode_branch(tasknode->task->fptr);*/
        create_context(tasknode->task->fptr, &end_task, tasknode->task->stack_top);
        return;
    }
}

taskCtrl* create_task(void (*fptr)(void), uint8_t priority, uint32_t ticks_until_wake) {
    taskCtrl *task;
    uint32_t *memory;

    task = (taskCtrl *) kmalloc(sizeof(taskCtrl));
    memory = (uint32_t *) malloc(STKSIZE*4, 1);

    task->stack_base = memory;
    task->stack_top  = memory + STKSIZE;
    task->fptr       = fptr;
    task->priority   = priority;
    task->ticks_until_wake = ticks_until_wake;
    task->running    = 0;

    return task;
}

void append_task_to_klist(taskNode* new_task){
    /* Check if head is set */
    if (task_list.head == NULL) {
        task_list.head = new_task;
        new_task->prev_node = NULL;
    }
    if (task_list.tail == NULL) {
        task_list.tail = new_task;
    }
    else {
        new_task->prev_node = task_list.tail;
        task_list.tail->next_node = new_task;
        task_list.tail = new_task;
    }

    new_task->next_node = NULL;
}

void append_task_to_queue(taskNode* new_task){
    taskNode* last_node = task_queue.tail;
    new_task->next_node = NULL;
    last_node->next_node = new_task;
}

void idle_task(void){
    while(1){
        __asm__("nop");
    }
}

void end_task(void){

    /* Raise privilege */
    __asm__("push {lr}");
    _svc(0);
    __asm__("pop {lr}");

    __asm__("push {lr}");
    disable_psp();
    __asm__("pop {lr}");

    __asm__("push {lr}");
    remove_task(k_currentTask);
    __asm__("pop {lr}");

    __asm__("push {lr}");
    switch_task();
    __asm__("pop {lr}");
    
    __asm__("push {lr}");
    enable_psp(k_currentTask->task->stack_top);
    restore_full_context();
    /* This shouldn't return */

    __asm__("pop {lr} \n"
            "bx lr\n");
}

void register_task(taskCtrl *task_ptr){
    taskNode* new_task = kmalloc(sizeof(taskNode));

    new_task->task = task_ptr;
    append_task_to_klist(new_task);
}

void remove_task(taskNode *tasknode) {
    /* Remove from end/middle/beginning of list */
    if (tasknode->next_node == NULL && tasknode->prev_node) {
        tasknode->prev_node->next_node = NULL;
    }
    if (tasknode->prev_node) {
        tasknode->prev_node->next_node = tasknode->next_node;
    }
    else {
        tasknode->next_node->prev_node = NULL;
    }

    /* Remove from head/tail of task_list */
    if (task_list.head == tasknode) {
        task_list.head = tasknode->next_node;
    }
    if (task_list.tail == tasknode) {
        task_list.tail = tasknode->prev_node;
    }
}

taskNodeList sort_by_priority(taskNodeList list){
    taskNodeList ret = {NULL, NULL};
    taskNode* temp_head = list.head;
    taskNode* ptr = list.head;
    taskNode* preptr = list.head;
    taskNode* pre_min_node = list.head;
    int8_t min = ptr->task->priority;
    while(ptr->next_node != NULL){
        if(min >= ptr->task->priority){
            min = ptr->task->priority;
            pre_min_node = preptr;
        }
        preptr = ptr;
        ptr = ptr->next_node;
        //derp
    }
    ret.head = pre_min_node->next_node;
    ret.tail = ret.head;
    while(temp_head->next_node != NULL){
        preptr = ptr;
        ptr = ptr->next_node;
        while(ptr->next_node != NULL){
            if(min >= ptr->task->priority){
                min = ptr->task->priority;
                pre_min_node = preptr;
            }
        }
        ret.tail->next_node = pre_min_node->next_node;
        ret.tail = ret.tail->next_node;
    }
    return ret;
}

taskNode* find_last_taskNode(taskNode* init_node){
    taskNode* next = init_node;
    while(next->next_node != NULL){
        next = next->next_node;
    }
    return next;
}

