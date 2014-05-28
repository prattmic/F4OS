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
#include <stdlib.h>
#include <string.h>
#include <mm/mm.h>
#include <kernel/mutex.h>
#include <kernel/sched.h>
#include <kernel/fault.h>
#include <mm/mm.h>
#include <dev/resource.h>

#include <dev/shared_mem.h>

#define SM_SIZE   CONFIG_SHARED_MEM_SIZE

typedef struct shared_mem {
        char        data[SM_SIZE];
        int         read_ctr;
        int         write_ctr;
} shared_mem;

char shared_mem_read(void *env, int *error) __attribute__((section(".kernel")));
int shared_mem_write(char c, void *env) __attribute__((section(".kernel")));
int shared_mem_close(resource *env) __attribute__((section(".kernel")));

rd_t open_shared_mem(void) {
    rd_t ret;

    shared_mem *mem = kmalloc(sizeof(shared_mem));
    if (!mem) {
        ret = -1;
        goto err;
    }

    resource *new_r = create_new_resource();
    if (!new_r) {
        ret = -1;
        goto err_free_mem;
    }

    mem->read_ctr = 0;
    mem->write_ctr = 0;
    memset(mem->data, '\0', SM_SIZE);

    new_r->env = mem;
    new_r->writer = &shared_mem_write;
    new_r->swriter = NULL;
    new_r->reader = &shared_mem_read;
    new_r->closer = &shared_mem_close;
    new_r->read_mut = kmalloc(sizeof(mutex));
    if (new_r->read_mut) {
        init_mutex(new_r->read_mut);
    }
    else {
        ret = -1;
        goto err_free_new_r;
    }
    new_r->write_mut = new_r->read_mut;

    ret = add_resource(curr_task, new_r);
    if (ret < 0) {
        goto err_free_new_r;
    }

    return ret;

err_free_new_r:
    kfree(new_r);
err_free_mem:
    kfree(mem);
err:
    printk("OOPS: Unable to open shared mem.\r\n");
    return ret;
}

char shared_mem_read(void *env, int *error) {
    if (error != NULL) {
        *error = 0;
    }

    shared_mem *mem = (shared_mem *)env;
    if(mem->read_ctr > 512) {
        mem->read_ctr = 1;
        return mem->data[0];
    }
    else return mem->data[mem->read_ctr++];
}

int shared_mem_write(char c, void *env) {
    shared_mem *mem = (shared_mem *)env;
    if(mem->write_ctr > 512) {
        mem->write_ctr = 1;
        mem->data[0] = c;
    }
    else mem->data[mem->write_ctr++] = c;

    return 1;
}

int shared_mem_close(resource *resource) {
    kfree(resource->env);
    acquire_for_free(resource->read_mut);
    kfree((void*) resource->read_mut);

    return 0;
}
