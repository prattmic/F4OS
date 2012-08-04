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

resource default_resources[RESOURCE_TABLE_SIZE] = {{&usart_puts, &usart_getc}
                                                  };

void add_resource(task_ctrl* tcs, resource* r) {
    tcs->resources[tcs->top_rd] = r;
    if(tcs->resources[tcs->top_rd++] == NULL) {
        printf("Resource add failed, resource is null");
    }
}

void resource_setup(task_ctrl* tcs) {
    tcs->top_rd = 0;
    resource *new_r = kmalloc(sizeof(resource));
    new_r->writer = &usart_puts;
    new_r->reader = &usart_getc;
    add_resource(tcs, new_r);
}

void write(rd_t rd, char* s) {
    if (rd >= RESOURCE_TABLE_SIZE) {
        panic_print("Resource descriptor too large");
    }

    if (task_switching) {
        curr_task->task->resources[rd]->writer(s);
    }
    else {
        default_resources[rd].writer(s);
    }
}

char read(rd_t rd) {
    if (rd >= RESOURCE_TABLE_SIZE) {
        panic_print("Resource descriptor too large");
    }

    if (task_switching) {
        return curr_task->task->resources[rd]->reader();
    }
    else {
        return default_resources[rd].reader();
    }
}
