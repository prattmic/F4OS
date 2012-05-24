#include "types.h"
#include "mem.h"
#include "task.h"
#include "heap.h"

taskCtl k_currentTask;
taskCtl k_idle_task;


taskNode sys_idle_task;
taskNodeList task_list;
taskNodeList task_queue;


void init_kernel(void){
    sys_idle_task.next_node = NULL;
    (sys_idle_task.task)->fptr =        &idle_task;
    (sys_idle_task.task)->stack_base =  IDLE_TASK_BASE;
    (sys_idle_task.task)->stack_top =   IDLE_TASK_BASE;
    

}

void append_task_to_klist(taskNode* new_task){
    taskNode* last_node = task_list.tail;
    new_task->next_node = NULL;
    last_node->next_node = new_task;
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

void register_task(void (*task_ptr)(void)){
    taskNode* new_task = malloc(sizeof(taskNode));
    taskNode* end_task = task_list.tail;
    end_task->next_node = new_task;
    
    //DOSHIT
    return;
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
}

taskNode* find_last_taskNode(taskNode* init_node){
    taskNode* next = init_node;
    while(next->next_node != NULL){
        next = next->next_node;
    }
    return next;
}

