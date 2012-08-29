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

resource default_resources[RESOURCE_TABLE_SIZE] = {{.writer = &usart_putc,
                                                    .reader = &usart_getc,
                                                    .closer = &usart_close,
                                                    .env    = NULL,
                                                    .sem    = &usart_semaphore},
                                                   {NULL}};

static inline uint8_t resource_null(resource *r) {
    if (r->writer == NULL && r->reader == NULL && r->closer == NULL && r->env == NULL && r->sem == NULL) {
        return 1;
    }

    return 0;
}

void add_resource(task_ctrl* tcs, resource* r) {
    tcs->resources[tcs->top_rd] = r;
    if(tcs->resources[tcs->top_rd++] == NULL) {
        printf("Resource add failed, resource is null\n");
    }
}

void resource_setup(task_ctrl* task) {
    /* Copy resources */
    if (task_switching) {
        for (int i = 0; i < RESOURCE_TABLE_SIZE; i++) {
            task->resources[i] = curr_task->task->resources[i];
        }

        task->top_rd = curr_task->task->top_rd;
    }
    else {
        int top_rd = 0;

        for (int i = 0; i < RESOURCE_TABLE_SIZE; i++) {
            if (!resource_null(&default_resources[i])) {
                task->resources[i] = &default_resources[i];
                top_rd++;
            }
            else {
                task->resources[i] = NULL;
            }
        }

        task->top_rd = top_rd;
    }
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
        if (rd == curr_task->task->top_rd - 1) {
            curr_task->task->top_rd--;
        }
        curr_task->task->resources[rd]->closer(curr_task->task->resources[rd]);
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
