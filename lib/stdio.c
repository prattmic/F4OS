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

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dev/buf_stream.h>
#include <dev/char.h>
#include <dev/resource.h>

int write(struct char_device *c, char *buf, int num) {
    struct char_ops *ops;

    if (!c) {
        return -1;
    }

    ops = c->obj.ops;

    return ops->write(c, buf, num);
}

int read(struct char_device *c, char *buf, int num) {
    struct char_ops *ops;

    if (!c) {
        return -1;
    }

    ops = c->obj.ops;

    return ops->read(c, buf, num);
}

int read_block(struct char_device *dev, char *buf, int num) {
    int ret;
    size_t total = 0;

    do {
        ret = read(dev, buf, num);

        /* Next time we start later in the buf, and have less to write */
        buf += ret;
        num -= ret;
        total += ret;
    } while (ret >= 0 && num > 0);

    /* Return bytes written unless there was an error */
    if (ret >= 0) {
        ret = total;
    }

    return ret;
}

int write_block(struct char_device *dev, char *buf, int num) {
    int ret;
    size_t total = 0;

    do {
        ret = write(dev, buf, num);

        /* Next time we start later in the buf, and have less to write */
        buf += ret;
        num -= ret;
        total += ret;
    } while (ret >= 0 && num > 0);

    /* Return bytes written unless there was an error */
    if (ret >= 0) {
        ret = total;
    }

    return ret;
}

int fputs(struct char_device *dev, char *s) {
    return write_block(dev, s, strlen(s));
}

int fputc(struct char_device *dev, char letter) {
    return write_block(dev, &letter, 1);
}

int fgetc(struct char_device *dev) {
    int ret;
    char c;

    ret = read_block(dev, &c, 1);

    /* Return character, unless there was an error */
    if (ret >= 0) {
        ret = c;
    }

    return ret;
}

int scnprintf(char *buf, uint32_t n, char *fmt, ...) {
    va_list ap;
    struct char_device *stream;
    int ret;

    va_start(ap, fmt);

    stream = buf_stream_create(buf, n);
    if (!stream) {
        ret = -1;
        goto out;
    }

    ret = vfprintf(stream, fmt, ap);

    obj_put(&stream->obj);
out:
    va_end(ap);
    return ret;
}

int fprintf(struct char_device *dev, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = vfprintf(dev, fmt, ap);
    va_end(ap);

    return ret;
}

static inline int holding_flush(char *holding, int *hold_count,
                                struct char_device *dev) {
    if (*hold_count) {
        int ret = fputs(dev, holding);
        if (ret >= 0) {
            *hold_count = 0;
            holding[0] = '\0';
        }
        return ret;
    }
    else {
        return 0;
    }
}

static inline int holding_push(char c, char *holding, int hold_len,
                               int *hold_count, struct char_device *dev) {
    int ret;

    if (*hold_count >= hold_len - 1) {
        ret = holding_flush(holding, hold_count, dev);
    }
    else {
        ret = 0;
    }

    holding[(*hold_count)++] = c;
    holding[*hold_count] = '\0';

    return ret;
}

/* Returns bytes written, negative on error */
int vfprintf(struct char_device *dev, char *fmt, va_list ap) {
    int total = 0;

    /*
     * We buffer data here as long as possible, because the actual puts/putc
     * functions tend to be slow and the fewer calls we can make, the better.
     */
    char holding[32];
    int hold_count = 0;
    const int hold_len = sizeof(holding)/sizeof(holding[0]);

    holding[0] = '\0';

    int ret;

    while (*fmt) {
        if (*fmt == '%') {
            switch (*(++fmt)) {
                case 'x': {
                    /* Hex */
                    uint32_t hex = va_arg(ap, uint32_t);
                    char buf[12];

                    uitoa(hex, buf, 9, 16);

                    ret = holding_flush(holding, &hold_count, dev);
                    if (ret >= 0) {
                        total += ret;
                    }
                    else {
                        return ret;
                    }

                    ret = fputs(dev, buf);
                    if (ret >= 0) {
                        total += ret;
                    }
                    else {
                        return ret;
                    }

                    break;
                }
                case 'i': case 'd': {
                    int num = va_arg(ap, int);
                    char buf[9];    /* 7 digits in INT_MAX + '-' and '\0' */

                    itoa(num, buf, 12, 10);

                    ret = holding_flush(holding, &hold_count, dev);
                    if (ret >= 0) {
                        total += ret;
                    }
                    else {
                        return ret;
                    }

                    ret = fputs(dev, buf);
                    if (ret >= 0) {
                        total += ret;
                    }
                    else {
                        return ret;
                    }
                    break;
                }
                case 'u': {
                    uint32_t num = va_arg(ap, uint32_t);
                    char buf[9];    /* 7 digits in INT_MAX + '-' and '\0' */

                    uitoa(num, buf, 12, 10);

                    ret = holding_flush(holding, &hold_count, dev);
                    if (ret >= 0) {
                        total += ret;
                    }
                    else {
                        return ret;
                    }

                    ret = fputs(dev, buf);
                    if (ret >= 0) {
                        total += ret;
                    }
                    else {
                        return ret;
                    }
                    break;
                }
                case 'f': {
                    float num = (float) va_arg(ap, double);
                    char buf[20];

                    ftoa(num, 0.0001f, buf, 20);

                    ret = holding_flush(holding, &hold_count, dev);
                    if (ret >= 0) {
                        total += ret;
                    }
                    else {
                        return ret;
                    }

                    ret = fputs(dev, buf);
                    if (ret >= 0) {
                        total += ret;
                    }
                    else {
                        return ret;
                    }
                    break;
                }
                case 'c': {
                    char letter = (char) va_arg(ap, uint32_t);

                    ret = holding_push(letter, holding, hold_len, &hold_count,
                                       dev);
                    if (ret >= 0) {
                        total += ret;
                    }
                    else {
                        return ret;
                    }

                    break;
                }
                case 's': {
                    char *s = va_arg(ap, char*);

                    ret = holding_flush(holding, &hold_count, dev);
                    if (ret >= 0) {
                        total += ret;
                    }
                    else {
                        return ret;
                    }

                    ret = fputs(dev, s);
                    if (ret >= 0) {
                        total += ret;
                    }
                    else {
                        return ret;
                    }

                    break;
                }
                case '%': { /* Just print a % */
                    ret = holding_push('%', holding, hold_len, &hold_count,
                                       dev);
                    if (ret >= 0) {
                        total += ret;
                    }
                    else {
                        return ret;
                    }

                    break;
                }
                default: {
                    ret = holding_push('%', holding, hold_len, &hold_count,
                                       dev);
                    if (ret >= 0) {
                        total += ret;
                    }
                    else {
                        return ret;
                    }

                    ret = holding_push(*fmt, holding, hold_len, &hold_count,
                                       dev);
                    if (ret >= 0) {
                        total += ret;
                    }
                    else {
                        return ret;
                    }
                }
            }

            fmt++;
        }
        else {
            ret = holding_push(*fmt++, holding, hold_len, &hold_count, dev);
            if (ret >= 0) {
                total += ret;
            }
            else {
                return ret;
            }
        }
    }

    ret = holding_flush(holding, &hold_count, dev);
    if (ret >= 0) {
        total += ret;
    }
    else {
        return ret;
    }

    return total;
}
