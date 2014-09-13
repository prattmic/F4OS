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
#include <mm/mm.h>
#include <kernel/sched.h>
#include <kernel/mutex.h>
#include <kernel/fault.h>
#include <dev/resource.h>

#include <dev/buf_stream.h>

struct buf_stream {
    char *buf;
    uint32_t i;
    uint32_t len;
};

static char buf_stream_read(void *env, int *error) {
    if (error != NULL) {
        *error = 0;
    }

    struct buf_stream *stream = (struct buf_stream *) env;

    if ((stream->i >= stream->len) || (stream->buf[stream->i] == '\0')) {
        return '\0';
    }

    return stream->buf[stream->i++];
}

static int buf_stream_write(char c, void *env) {
    struct buf_stream *stream = (struct buf_stream *) env;

    /* Always leave room for NULL byte at end of buffer */
    if (stream->i >= stream->len - 1) {
        return 0;
    }

    stream->buf[stream->i++] = c;
    stream->buf[stream->i] = '\0';
    return 1;
}

static int buf_stream_close(resource *resource) {
    acquire_for_free(resource->read_mut);
    free(resource->env);
    free((void*) resource->read_mut);
    return 0;
}

struct resource *open_buf_stream(char *buf, uint32_t len) {
    struct buf_stream *env = malloc(sizeof(struct buf_stream));
    if (!env) {
        goto err;
    }

    resource *new_r = create_new_resource();
    if (!new_r) {
        goto err_free_env;
    }

    env->buf = buf;
    env->i = 0;
    env->len = len;

    new_r->env    = env;
    new_r->writer = &buf_stream_write;
    new_r->swriter = NULL;
    new_r->reader = &buf_stream_read;
    new_r->closer = &buf_stream_close;
    new_r->read_mut = malloc(sizeof(mutex));
    if (new_r->read_mut) {
        init_mutex(new_r->read_mut);
    }
    else {
        goto err_free_new_r;
    }
    new_r->write_mut = new_r->read_mut;

    return new_r;

err_free_new_r:
    kfree(new_r);
err_free_env:
    kfree(env);
err:
    printk("OOPS: Unable to open buffer stream.\r\n");
    return NULL;
}
