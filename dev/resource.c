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
#include <kernel/fault.h>
#include <kernel/init.h>
#include <kernel/mutex.h>
#include <kernel/sched.h>
#include <mm/mm.h>

resource *create_new_resource(void) {
    resource *ret = kmalloc(sizeof(resource));
    if (ret) {
        memset(ret, 0, sizeof(resource));
    }
    return ret;
}

int resource_close(struct resource *resource) {
    int ret;

    if (!resource) {
        return -1;
    }

    ret = resource->closer(resource);
    if (!ret) {
        goto out;
    }

    kfree(resource);

out:
    return ret;
}

/*
 * Setup new task IO
 *
 * This means copying stdin/stdout/stderr from the current task.
 */
void task_resource_setup(task_t *task) {
    if (stdin) {
        obj_get(&stdin->obj);
    }
    task->_stdin = stdin;

    if (stdout) {
        obj_get(&stdout->obj);
    }
    task->_stdout = stdout;

    if (stderr) {
        obj_get(&stderr->obj);
    }
    task->_stderr = stderr;
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

/*
 * Even though read() and write() should not block,
 * we have no choice, because the underlying resource
 * read() and write() do block.
 */
static struct char_ops resource_ops = {
    .read = resource_read,
    .write = resource_write,
    ._cleanup = resource_cleanup,
};

struct char_device *resource_to_char_device(struct resource *resource) {
    struct char_device *c;

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

int resource_char_device_equal(struct char_device *d1,
                               struct char_device *d2) {
    return d1->priv == d2->priv;
}
