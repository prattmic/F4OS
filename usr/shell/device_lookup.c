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

#include <stddef.h>
#include <stdio.h>
#include <dev/device.h>
#include <kernel/obj.h>

const char *device_auto_lookup(struct class *class) {
    const char *driver = NULL;
    int total;

    /* Hopefully, there is only one driver */
    total = device_list_class(class, &driver, 1);
    if (total < 1) {
        printf("Error: No devices found.\r\n");
        return NULL;
    }
    else if (total > 1) {
        const char *names[total];
        /* Now, get all of the drivers */
        total = device_list_class(class, names, total);

        printf("Multiple devices found:\r\n");
        for (int i = 0; i < total; i++) {
            printf("* %s\r\n", names[i]);
        }

        return NULL;
    }

    return driver;
}
