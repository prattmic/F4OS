#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mm/mm.h>
#include <kernel/sched.h>
#include <kernel/semaphore.h>
#include <kernel/fault.h>

#include <dev/resource.h>

static inline uint8_t resource_null(resource *r) {
    if (r->writer == NULL && r->swriter == NULL && r->reader == NULL && r->closer == NULL && r->env == NULL && r->sem == NULL) {
        return 1;
    }

    return 0;
}

resource *create_new_resource(void) {
    resource *ret = kmalloc(sizeof(resource));
    if(ret) {
        memset(ret, 0, sizeof(resource));
    }
    return ret;
}

rd_t add_resource(task_ctrl* tcs, resource* r) {
    if(r == NULL) {
        panic_print("Cannot add NULL resource.");
    }

    /* There is room at the end of the resource table, so add there */
    if (tcs->top_rd < RESOURCE_TABLE_SIZE) {
        rd_t rd = tcs->top_rd;

        tcs->resources[rd] = r;
        tcs->top_rd++;

        return rd;
    }
    /* There is no room at end of resource table, so search for space */
    else {
        /* We only search up to tcs->top_rd.  Since we are here, that
         * should be the end of the list, but just to be safe, we want
         * to make sure we don't find a space after tcs->top_rd and then
         * fail to increment tcs->top_rd */
        for (int i = 0; i < tcs->top_rd; i++) {
            /* Found an empty space! */
            if (tcs->resources[i] == NULL) {
                rd_t rd = i;

                tcs->resources[rd] = r;

                return rd;
            }
        }

        /* If we got here, nothing was found. */
        panic_print("No room to add resources.");
    }

    return -1;
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
            if (default_resources[i]) {
                task->resources[i] = default_resources[i];
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
        if(default_resources[rd] != NULL) {
            for(int i = 0; i < n; i++) {
                default_resources[rd]->writer(d[i], default_resources[rd]->env);
            }
        }
    }
}

void swrite(rd_t rd, char* s) {
    if (rd >= RESOURCE_TABLE_SIZE) {
        panic_print("Resource descriptor too large");
    }
    if (task_switching) {
        acquire(curr_task->task->resources[rd]->sem);
        if (curr_task->task->resources[rd]->swriter) {
            curr_task->task->resources[rd]->swriter(s, curr_task->task->resources[rd]->env);
        }
        else {
            while(*s) {
                curr_task->task->resources[rd]->writer(*s++, curr_task->task->resources[rd]->env);
            }
        }
        release(curr_task->task->resources[rd]->sem);
    }
    else {
        if(default_resources[rd] != NULL) {
            if (default_resources[rd]->swriter) {
                default_resources[rd]->swriter(s, default_resources[rd]->env);
            }
            else {
                while(*s) {
                    default_resources[rd]->writer(*s++, default_resources[rd]->env);
                }
            }
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
        if(default_resources[rd] != NULL) {
            default_resources[rd]->closer(default_resources[rd]->env);
            kfree(default_resources[rd]);
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
        if(default_resources[rd] != NULL) {
            for(int i = 0; i < n; i++) {
                buf[i] = default_resources[rd]->reader(default_resources[rd]->env);
            }
        }
    }
}
