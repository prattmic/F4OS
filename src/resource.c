#include "types.h"
#include "registers.h"
#include "interrupt.h"
#include "usart.h"
#include "task.h"
#include "context.h"
#include "semaphore.h"
#include "buddy.h"
#include "resource.h"
#include "mem.h"

pipe_list k_pipes = {NULL, NULL};

void add_resource(task_ctrl* tcs, resource* r) {
    tcs->resources[tcs->top_rd] = r;
    if(tcs->resources[tcs->top_rd++] == NULL)
        printf("Resource add failed, resource is null");
}

void resource_setup(task_ctrl* tcs) {
    tcs->top_rd = 0;
    resource* new_r = kmalloc(sizeof(resource));
    new_r->addr = NULL;
    new_r->writer = &puts;
    new_r->reader = &getc;
    add_resource(tcs, new_r);
}

void write(rd_t rd, char* s) {
    k_curr_task->task->resources[rd]->writer(s);
}

pipe create_pipe(uint32_t reader_pid, uint32_t writer_pid) {
    pipe* new_pipe = kmalloc(sizeof(pipe));
    if(new_pipe == NULL)
        printf("Pipe allocation failed.");
    new_pipe->reader_pid = reader_pid;
    new_pipe->writer_pid = writer_pid;
    pipe_node* new_node = kmalloc(sizeof(pipe));
    if(new_node == NULL)
        printf("Pipe node allocation failed.");
    new_node->curr = new_pipe;
    add_pipe_to_list(new_node);
}

void add_pipe_to_list(pipe_node* p) {
    if(k_pipes.head == NULL) {
        k_pipes.head = p;
        k_pipes.tail = p;
        p->next = NULL;
        p->prev = NULL;
    }
    else {
        p->next = k_pipes.head;
        k_pipes.head = p;
        p->next->prev = p;
        p->prev = NULL;
    }
}

