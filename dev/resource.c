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
        printk("OOPS: NULL resource passed to add_resource.\r\n");
        return -1;
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
        printk("No room to add resources.\r\n");
        return -1;
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

/* Return bytes written, negative on error */
int write(rd_t rd, char* d, int n) {
    if (rd < 0 || rd >= RESOURCE_TABLE_SIZE) {
        return -1;
    }

    int tot = 0;

    if (task_switching) {
        acquire(curr_task->task->resources[rd]->sem);
        for(int i = 0; i < n; i++) {
            int ret = curr_task->task->resources[rd]->writer(d[i], curr_task->task->resources[rd]->env);
            if (ret > 0) {
                tot += ret;
            }
            else {
                /* Return on error */
                tot = ret;
                break;
            }
        }
        release(curr_task->task->resources[rd]->sem);
    }
    else {
        if(default_resources[rd] != NULL) {
            for(int i = 0; i < n; i++) {
                int ret = default_resources[rd]->writer(d[i], default_resources[rd]->env);
                if (ret >= 0) {
                    tot += ret;
                }
                else {
                    /* Return on error */
                    tot = ret;
                    break;
                }
            }
        }
        else {
            return -1;
        }
    }

    return tot;
}

/* Return bytes written, negative on error */
int swrite(rd_t rd, char* s) {
    if (rd < 0 || rd >= RESOURCE_TABLE_SIZE) {
        return -1;
    }

    int n = 0; 

    if (task_switching) {
        acquire(curr_task->task->resources[rd]->sem);
        if (curr_task->task->resources[rd]->swriter) {
            int ret = curr_task->task->resources[rd]->swriter(s, curr_task->task->resources[rd]->env);
            if (ret >= 0) {
                n += ret;
            }
            else {
                n = ret;
            }
                
        }
        else {
            while(*s) {
                int ret = curr_task->task->resources[rd]->writer(*s++, curr_task->task->resources[rd]->env);
                if (ret >= 0) {
                    n += ret;
                }
                else {
                    n = ret;
                    break;
                }
            }
        }
        release(curr_task->task->resources[rd]->sem);
    }
    else {
        if(default_resources[rd] != NULL) {
            if (default_resources[rd]->swriter) {
                int ret = default_resources[rd]->swriter(s, default_resources[rd]->env);
                if (ret >= 0) {
                    n += ret;
                }
                else {
                    n = ret;
                }
            }
            else {
                while(*s) {
                    int ret = default_resources[rd]->writer(*s++, default_resources[rd]->env);
                    if (ret >= 0) {
                        n += ret;
                    }
                    else {
                        n = ret;
                        break;
                    }
                }
            }
        }
        else { 
            return -1;
        }
    }

    return n;
}

/* Returns 0 on success, else on error */
int close(rd_t rd) {
    if (rd < 0 || rd >= RESOURCE_TABLE_SIZE) {
        return -1;
    }

    int ret;

    if (task_switching) {
        ret = curr_task->task->resources[rd]->closer(curr_task->task->resources[rd]);
        if (ret >= 0) {
            if (rd == curr_task->task->top_rd - 1) {
                curr_task->task->top_rd--;
            }
            kfree(curr_task->task->resources[rd]);
            curr_task->task->resources[rd] = NULL;
        }
    }
    else {
        if(default_resources[rd] != NULL) {
            ret = default_resources[rd]->closer(default_resources[rd]->env);
            if (ret >= 0) {
                kfree(default_resources[rd]);
                default_resources[rd] = NULL;
            }
        }
        else {
            ret = -1;
        }
    }

    return ret;
}

/* Returns number of bytes read, or negative on error */
int read(rd_t rd, char *buf, int n) {
    if (rd < 0 || rd >= RESOURCE_TABLE_SIZE) {
        return -1;
    }

    int tot = 0;

    if (task_switching) {
        acquire(curr_task->task->resources[rd]->sem);
        for(int i = 0; i < n; i++) {
            int error;
            buf[i] = curr_task->task->resources[rd]->reader(curr_task->task->resources[rd]->env, &error);
            if (!error) {
                tot += 1;
            }
            else {
                tot = error;
                break;
            }
        }
        release(curr_task->task->resources[rd]->sem);
    }
    else {
        if(default_resources[rd] != NULL) {
            for(int i = 0; i < n; i++) {
                int error;
                buf[i] = default_resources[rd]->reader(default_resources[rd]->env, &error);
                if (!error) {
                    tot += 1;
                }
                else {
                    tot = error;
                    break;
                }
            }
        }
    }

    return tot;
}
