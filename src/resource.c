#include "types.h"
#include "registers.h"
#include "interrupt.h"
#include "usart.h"
#include "task.h"
#include "context.h"
#include "semaphore.h"
#include "buddy.h"
#include "resource.h"
#include "stdio.h"
#include "mem.h"

resource default_resources[RESOURCE_TABLE_SIZE] = {{.env = NULL, .writer = &usart_putc, .reader = &usart_getc}
                                                  };

void add_resource(task_ctrl* tcs, resource* r) {
    tcs->resources[tcs->top_rd] = r;
    if(tcs->resources[tcs->top_rd++] == NULL)
        printf("Resource add failed, resource is null\n");
}

void resource_setup(task_ctrl* tcs) {
    tcs->top_rd = 0;
    resource* new_r = kmalloc(sizeof(resource));
    new_r->writer = &usart_putc;
    new_r->reader = &usart_getc;
    new_r->env = NULL;
    new_r->sem = &usart_semaphore;
    add_resource(tcs, new_r);
}

void write(rd_t rd, char* d, int n) {
    if (rd >= RESOURCE_TABLE_SIZE) {
        panic_print("Resource descriptor too large");
    }
    if (task_switching) {
        acquire(curr_task->task->resources[rd]->sem);
        for(int i = 0; i < n; i++) {
            curr_task->task->resources[rd]->writer(d[i], curr_task->task->resources[rd]->env);
        }
        release(curr_task->task->resources[rd]->sem);
    }
    else {
        for(int i = 0; i < n; i++) {
            default_resources[rd].writer(d[i], default_resources[rd].env);
        }
    }
}

void swrite(rd_t rd, char* s) {
    if (rd >= RESOURCE_TABLE_SIZE) {
        panic_print("Resource descriptor too large");
    }
    if (task_switching) {
        acquire(curr_task->task->resources[rd]->sem);
        while(*s) {
            curr_task->task->resources[rd]->writer(*s++, curr_task->task->resources[rd]->env);
        }
        release(curr_task->task->resources[rd]->sem);
    }
    else {
        while(*s) {
            default_resources[rd].writer(*s++, default_resources[rd].env);
        }
    }
}

void read(rd_t rd, char *buf, int n) {
    if (rd >= RESOURCE_TABLE_SIZE) {
        panic_print("Resource descriptor too large");
    }
    if (task_switching) {
        acquire(curr_task->task->resources[rd]->sem);
        for(int i = 0; i < n; i++) {
            buf[i] = curr_task->task->resources[rd]->reader(curr_task->task->resources[rd]->env);
        }
        release(curr_task->task->resources[rd]->sem);
    }
    else {
        for(int i = 0; i < n; i++) {
            buf[i] = default_resources[rd].reader(default_resources[rd].env);
        }
    }
}
