/*
 * Copyright (C) 2013, 2014 F4OS Authors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dev/resource.h>
#include <dev/char.h>
#include <mm/mm.h>
#include <kernel/sched.h>
#include <kernel/mutex.h>
#include <kernel/fault.h>

/* Default resources used before task switching, and inherited to all tasks */
resource *default_resources[RESOURCE_TABLE_SIZE] = {
    [stdout] = STDOUT_DEV,
    [stderr] = STDERR_DEV,
};

/* Get resource struct from rd */
static inline resource *get_resource(rd_t rd) {
    if (rd < 0 || rd >= RESOURCE_TABLE_SIZE) {
        return NULL;
    }

    /* Before task switching begins, the default list of resources must be used.
     * Once it begins, we can get the resouece directly from the task. */
    if (task_switching) {
        return curr_task->resource_data.resources[rd];
    }
    else {
        return default_resources[rd];
    }
}

resource *create_new_resource(void) {
    resource *ret = kmalloc(sizeof(resource));
    if (ret) {
        memset(ret, 0, sizeof(resource));
    }
    return ret;
}

/* Add new resource to task */
rd_t add_resource(task_t *task, resource* r) {
    if (r == NULL) {
        printk("OOPS: NULL resource passed to add_resource.\r\n");
        return -1;
    }

    struct task_resource_data *task_data = &task->resource_data;

    /* There is room at the end of the resource table, so add there */
    if (task_data->top_rd < RESOURCE_TABLE_SIZE) {
        rd_t rd = task_data->top_rd;

        task_data->resources[rd] = r;
        task_data->top_rd++;

        return rd;
    }
    /* There is no room at end of resource table, so search for space */
    else {
        /* We only search up to tcs->top_rd.  Since we are here, that
         * should be the end of the list, but just to be safe, we want
         * to make sure we don't find a space after tcs->top_rd and then
         * fail to increment tcs->top_rd */
        for (int i = 0; i < task_data->top_rd; i++) {
            /* Found an empty space! */
            if (task_data->resources[i] == NULL) {
                rd_t rd = i;

                task_data->resources[rd] = r;

                return rd;
            }
        }

        /* If we got here, nothing was found. */
        printk("OOPS: No room to add resources.\r\n");
        return -1;
    }
}

/* Properly set up resources in a new task.
 * This means copying the resources from the current task,
 * or the default resource list, if task switching has not
 * yet begun. */
void task_resource_setup(task_t *task) {
    struct task_resource_data *task_data = &task->resource_data;

    if (task_switching) {
        struct task_resource_data *curr_task_data = &curr_task->resource_data;

        for (int i = 0; i < RESOURCE_TABLE_SIZE; i++) {
            task_data->resources[i] = curr_task_data->resources[i];
        }

        task_data->top_rd = curr_task_data->top_rd;
    }
    else {
        int top_rd = 0;

        for (int i = 0; i < RESOURCE_TABLE_SIZE; i++) {
            if (default_resources[i]) {
                task_data->resources[i] = default_resources[i];
                top_rd++;
            }
            else {
                task_data->resources[i] = NULL;
            }
        }

        task_data->top_rd = top_rd;
    }
}

/* Returns 0 on success, else on error */
int close(rd_t rd) {
    if (rd < 0 || rd >= RESOURCE_TABLE_SIZE) {
        return -1;
    }

    resource **resource = task_switching ?
        &curr_task->resource_data.resources[rd] : &default_resources[rd];

    if (!*resource) {
        return -1;
    }

    int ret = (*resource)->closer(*resource);
    if (!ret) {
        kfree(*resource);
        *resource = NULL;
        if (task_switching && (rd == curr_task->resource_data.top_rd - 1)) {
            curr_task->resource_data.top_rd--;
        }
    }

    return ret;
}

static int resource_read(struct char_device *c, char *buf, size_t num) {
    struct resource *resource = c->priv;
    int total = 0;

    acquire(resource->read_mut);

    for(int i = 0; i < num; i++) {
        int error;

        buf[i] = resource->reader(resource->env, &error);

        if (!error) {
            total += 1;
        }
        else {
            total = error;
            break;
        }
    }

    release(resource->read_mut);

    return total;
}

static int resource_write(struct char_device *c, char *buf, size_t num) {
    struct resource *resource = c->priv;
    int total = 0;

    acquire(resource->write_mut);

    /* swriter() is completely unused */

    for (int i = 0; i < num; i++) {
        int n = resource->writer(buf[i], resource->env);
        if (n < 0) { /* Error */
            total = n;
            break;
        }
        else if (n == 0) { /* Can't write anymore */
            break;
        }

        total++;
    }

    release(resource->write_mut);

    return total;
}

static int resource_cleanup(struct char_device *c) {
    /* Nothing to cleanup */
    return 0;
}

static struct char_ops resource_ops = {
    .read = resource_read,
    .write = resource_write,
    ._cleanup = resource_cleanup,
};

struct char_device *resource_to_char_device(rd_t r) {
    struct char_device *c;
    struct resource *resource = get_resource(r);
    if (!resource) {
        return NULL;
    }

    c = char_device_create(NULL, &resource_ops);
    if (!c) {
        return NULL;
    }

    c->priv = resource;

    return c;
}

int write(rd_t rd, char *buf, int n) {
    struct char_device *c;
    struct char_ops *ops;
    int ret;

    /* Adapt to a char device temporarily */
    c = resource_to_char_device(rd);
    if (!c) {
        return -1;
    }

    ops = c->obj.ops;

    ret = ops->write(c, buf, n);

    obj_put(&c->obj);

    return ret;
}

int swrite(rd_t rd, char* s) {
    return write(rd, s, strlen(s));
}

int read(rd_t rd, char *buf, int n) {
    struct char_device *c;
    struct char_ops *ops;
    int ret;

    /* Adapt to a char device temporarily */
    c = resource_to_char_device(rd);
    if (!c) {
        return -1;
    }

    ops = c->obj.ops;

    ret = ops->read(c, buf, n);

    obj_put(&c->obj);

    return ret;
}
