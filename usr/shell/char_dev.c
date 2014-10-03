/*
 * Copyright (C) 2014 F4OS Authors
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
#include <stdlib.h>
#include <dev/char.h>
#include <kernel/obj.h>
#include "app.h"

/* Simple program to provide a passthrough for any char_device */

void char_passthrough(struct char_device *dev) {
    char read_from_char[64] = { '\0' };
    char read_from_stdin[64] = { '\0' };
    int ret;

    while (1) {
        ret = read(dev, read_from_char, 64);
        if (ret < 0) {
            fprintf(stderr, "Failed to read from dev: %d\r\n", ret);
            return;
        }
        else if (ret > 0) {
            write_block(stdout, read_from_char, ret);
        }

        ret = read(stdin, read_from_stdin, 64);
        if (ret < 0) {
            fprintf(stderr, "Failed to read from stdin: %d\r\n", ret);
            return;
        }
        else if (ret > 0) {
            write_block(dev, read_from_stdin, ret);
        }
    }
}

void char_dev(int argc, char **argv) {
    struct char_device *dev;

    if (argc < 2 || argc > 3) {
        printf("Usage: %s device\r\n", argv[0]);
        return;
    }

    dev = char_device_get(argv[1]);
    if (!dev) {
        fprintf(stderr, "Unable to find device '%s'\r\n", argv[1]);
        return;
    }

    char_passthrough(dev);

    char_device_put(dev);
}
DEFINE_APP(char_dev)
