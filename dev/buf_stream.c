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
#include <kernel/mutex.h>
#include <dev/buf_stream.h>
#include <dev/char.h>

struct buf_stream {
    char *buf;
    uint32_t i;
    uint32_t len;
    struct mutex lock;
};

static int buf_stream_read(struct char_device *dev, char *buf, size_t num) {
    struct buf_stream *stream = dev->priv;
    int i, total = 0;

    acquire(&stream->lock);

    for (i = 0; i < num; i++) {
        /* Nothing to read */
        if (stream->i >= stream->len) {
            break;
        }

        buf[i] = stream->buf[stream->i];
        total++;

        /* Nothing after this to read */
        if (stream->buf[stream->i] == '\0') {
            break;
        }

        stream->i++;
    }

    release(&stream->lock);

    return total;
}

static int buf_stream_write(struct char_device *dev, const char *buf,
                            size_t num) {
    struct buf_stream *stream = dev->priv;
    int i, total = 0;

    acquire(&stream->lock);

    for (i = 0; i < num; i++) {
        /* Always leave room for NULL byte at end of buffer */
        if (stream->i >= stream->len - 1) {
            break;
        }

        stream->buf[stream->i++] = buf[i];
        stream->buf[stream->i] = '\0';
        total++;
    }

    release(&stream->lock);

    return total;
}

static int buf_stream_cleanup(struct char_device *dev) {
    free(dev->priv);
    return 0;
}

static struct char_ops buf_stream_ops = {
    .read = buf_stream_read,
    .write = buf_stream_write,
    ._cleanup = buf_stream_cleanup,
};

struct char_device *buf_stream_create(char *buf, uint32_t len) {
    struct char_device *dev;
    struct buf_stream *stream;

    stream = malloc(sizeof(*stream));
    if (!stream) {
        goto err;
    }

    dev = char_device_create(NULL, &buf_stream_ops);
    if (!dev) {
        goto err_free_stream;
    }

    stream->buf = buf;
    stream->i = 0;
    stream->len = len;
    init_mutex(&stream->lock);

    dev->priv = stream;

    return dev;

err_free_stream:
    free(stream);
err:
    return NULL;
}
