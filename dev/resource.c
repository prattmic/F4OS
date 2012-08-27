#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <mm/mm.h>
#include <kernel/sched.h>
#include <kernel/semaphore.h>
#include <kernel/fault.h>
#include <dev/hw/usart.h>

#include <dev/resource.h>

resource default_resources[RESOURCE_TABLE_SIZE] = {{.env = NULL, .writer = &usart_putc, .reader = &usart_getc}};

void add_resource(task_ctrl* tcs, resource* r) {
    tcs->resources[tcs->top_rd] = r;
    if(tcs->resources[tcs->top_rd++] == NULL) {
        printf("Resource add failed, resource is null\n");
    }
}

void resource_setup(task_ctrl* tcs) {
    for (int i = 0; i < RESOURCE_TABLE_SIZE; i++) {
        tcs->resources[i] = NULL;
    }

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

void close(rd_t rd) {
    if (rd >= RESOURCE_TABLE_SIZE) {
        panic_print("Resource descriptor too large");
    }
    if (task_switching) {
        acquire(curr_task->task->resources[rd]->sem);
        curr_task->task->resources[rd]->closer(curr_task->task->resources[rd]->env);
        kfree(curr_task->task->resources[rd]);
        curr_task->task->resources[rd] = NULL;
    }
    else {
        default_resources[rd].closer(default_resources[rd].env);
        kfree(&default_resources[rd]);
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
