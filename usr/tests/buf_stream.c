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

#include <stdio.h>
#include <string.h>
#include <dev/buf_stream.h>
#include <dev/char.h>
#include "test.h"

#define STREAM_MESSAGE "The Guide is definitive. Reality is frequently inaccurate."
#define BUF_LEN 128

static int buf_stream_write_test(char *message, int len) {
    int ret, written;
    char buf[BUF_LEN];
    struct char_device *stream;

    stream = buf_stream_create(buf, BUF_LEN);
    if (!stream) {
        strncpy(message, "Unable to open buf stream", len);
        ret = FAILED;
        goto out;
    }

    written = fputs(stream, STREAM_MESSAGE);
    if (written != ARRAY_LENGTH(STREAM_MESSAGE)-1) {
        strncpy(message, "Incorrect number of characters written", len);
        ret = FAILED;
        goto out_put;
    }

    if (strncmp(buf, STREAM_MESSAGE, BUF_LEN) != 0) {
        /* Avoid using scnprintf, which will end up using buffer streams */
        strncpy(message, "Buffer does not match written contents.", len);
        ret = FAILED;
        goto out_put;
    }

    ret = PASSED;

out_put:
    obj_put(&stream->obj);
out:
    return ret;
}
DEFINE_TEST("Buffer stream write", buf_stream_write_test);

static int buf_stream_read_test(char *message, int len) {
    int ret;
    char buf[BUF_LEN] = STREAM_MESSAGE;
    char read_buf[BUF_LEN];
    struct char_device *stream;
    int i = 0;

    stream = buf_stream_create(buf, BUF_LEN);
    if (!stream) {
        strncpy(message, "Unable to open buf stream", len);
        ret = FAILED;
        goto out;
    }

    while ((read_buf[i++] = fgetc(stream)) && (i < BUF_LEN));

    if (strncmp(read_buf, STREAM_MESSAGE, BUF_LEN) != 0) {
        /* Avoid using scnprintf, which will end up using buffer streams */
        strncpy(message, "Read characters do not match original buffer", len);
        ret = FAILED;
        goto out_put;
    }

    ret = PASSED;

out_put:
    obj_put(&stream->obj);
out:
    return ret;
}
DEFINE_TEST("Buffer stream read", buf_stream_read_test);

/* It should not be possible to write to a length 1 buffer,
 * as the last space is left for a NULL char */
static int buf_stream_single_char_test(char *message, int len) {
    int ret, written;
    struct char_device *stream;
    char c = '\0';

    stream = buf_stream_create(&c, 1);
    if (!stream) {
        strncpy(message, "Unable to open buf stream", len);
        ret = FAILED;
        goto out;
    }

    written = fputs(stream, STREAM_MESSAGE);
    if (written != 0) {
        strncpy(message, "Incorrect number of characters written", len);
        ret = FAILED;
        goto out_put;
    }

    if (c != '\0') {
        /* Avoid using scnprintf, which will end up using buffer streams */
        strncpy(message, "Stream overwrote character", len);
        ret = FAILED;
        goto out_put;
    }

    ret = PASSED;

out_put:
    obj_put(&stream->obj);
out:
    return ret;
}
DEFINE_TEST("Buffer stream single character", buf_stream_single_char_test);

#define SHORT_BUF_LEN   20

/* Try to write past end of buffer */
static int buf_stream_overfill_test(char *message, int len) {
    int ret, written;
    char buf[SHORT_BUF_LEN];
    struct char_device *stream;

    stream = buf_stream_create(buf, SHORT_BUF_LEN);
    if (!stream) {
        strncpy(message, "Unable to open buf stream", len);
        ret = FAILED;
        goto out;
    }

    written = fputs(stream, STREAM_MESSAGE);
    if (written != SHORT_BUF_LEN-1) {
        strncpy(message, "Incorrect number of characters written", len);
        ret = FAILED;
        goto out_put;
    }

    if (buf[SHORT_BUF_LEN-1] != '\0') {
        /* Avoid using scnprintf, which will end up using buffer streams */
        strncpy(message, "Terminating NULL overwritten", len);
        ret = FAILED;
        goto out_put;
    }

    ret = PASSED;

out_put:
    obj_put(&stream->obj);
out:
    return ret;
}
DEFINE_TEST("Buffer stream overfill", buf_stream_overfill_test);
