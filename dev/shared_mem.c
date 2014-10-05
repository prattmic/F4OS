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
#include <dev/char.h>
#include <dev/shared_mem.h>
#include <kernel/mutex.h>

#define SM_SIZE   CONFIG_SHARED_MEM_SIZE

struct shared_mem {
    char data[SM_SIZE];
    int read_ctr;
    int write_ctr;
    struct mutex lock;
};

static int shared_mem_read(struct char_device *dev, char *buf, size_t num) {
    struct shared_mem *mem;
    int total = 0;

    if (!dev) {
        return -1;
    }

    mem = dev->priv;

    acquire(&mem->lock);

    for (int i = 0; i < num; i++) {
        if (mem->read_ctr >= SM_SIZE) {
            mem->read_ctr = 0;
        }

        buf[i] =  mem->data[mem->read_ctr++];
        total++;
    }

    release(&mem->lock);

    return total;
}

static int shared_mem_write(struct char_device *dev, const char *buf,
                            size_t num) {
    struct shared_mem *mem;
    int total = 0;

    if (!dev) {
        return -1;
    }

    mem = dev->priv;

    acquire(&mem->lock);

    for (int i = 0; i < num; i++) {
        if (mem->write_ctr >= SM_SIZE) {
            mem->write_ctr = 0;
        }

        mem->data[mem->write_ctr++] = buf[i];
        total++;
    }

    release(&mem->lock);

    return total;
}

static int shared_mem_cleanup(struct char_device *dev) {
    free(dev->priv);
    return 0;
}

static struct char_ops shared_mem_ops = {
    .read = shared_mem_read,
    .write = shared_mem_write,
    ._cleanup = shared_mem_cleanup,
};

struct char_device *shared_mem_create(void) {
    struct char_device *dev;
    struct shared_mem *mem;

    mem = malloc(sizeof(*mem));
    if (!mem) {
        goto err;
    }

    dev = char_device_create(NULL, &shared_mem_ops);
    if (!dev) {
        goto err_free_mem;
    }

    mem->read_ctr = 0;
    mem->write_ctr = 0;
    memset(mem->data, '\0', SM_SIZE);
    init_mutex(&mem->lock);

    dev->priv = mem;

    return dev;

err_free_mem:
    free(mem);
err:
    return NULL;
}
