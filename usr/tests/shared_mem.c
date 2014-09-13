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
#include <kernel/sched.h>
#include <dev/char.h>
#include <dev/resource.h>
#include <dev/shared_mem.h>
#include "test.h"

#define IPC_MESSAGE "'Forty-two,' said Deep Thought, with infinite majesty and calm."

volatile int passed = 0;
struct resource *shared_mem;
struct char_device *dev;

static void memreader(void) {
    char buf[100] = {'\0'};

    read(dev, buf, 99);

    obj_put(&dev->obj);
    resource_close(shared_mem);

    if (strncmp(buf, IPC_MESSAGE, 100) == 0) {
        passed = 1;
    }
    else {
        passed = -1;
    }
}

static int shared_mem_test(char *message, int len) {
    shared_mem = open_shared_mem();
    if (!shared_mem) {
        strncpy(message, "Unable to open shared mem.", len);
        return FAILED;
    }

    dev = resource_to_char_device(shared_mem);
    if (!dev) {
        strncpy(message, "Unable to convert shared mem.", len);
        goto err;
    }

    swrite(dev, IPC_MESSAGE);
    new_task(&memreader, 1, 0);

    int count = 100000;
    while (!passed && (count-- > 0));

    if (passed == 1) {
        return PASSED;
    }
    else if (passed == -1) {
        strncpy(message, "Message mismatch", len);
    }
    else {
        strncpy(message, "Timed out", len);
    }

    return FAILED;

err:
    resource_close(shared_mem);
    return FAILED;
}
DEFINE_TEST("Shared memory resource", shared_mem_test);
