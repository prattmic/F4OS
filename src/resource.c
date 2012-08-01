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

void add_resource(task_ctrl* tcs, resource* r) {
    tcs->resources[tcs->top_rd] = r;
    if(tcs->resources[tcs->top_rd++] == NULL) {
        printf("Resource add failed, resource is null");
    }
}

void resource_setup(task_ctrl* tcs) {
    tcs->top_rd = 0;
    resource* new_r = kmalloc(sizeof(resource));
    new_r->writer = &puts;
    new_r->reader = &getc;
    add_resource(tcs, new_r);
}

void write(rd_t rd, char* s) {
    k_curr_task->task->resources[rd]->writer(s);
}

