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

#include <stdlib.h>
#include <stdio.h>
#include <dev/device.h>
#include <dev/accel.h>
#include <kernel/obj.h>
#include <math.h>
#include "device_lookup.h"
#include "app.h"

void accel(int argc, char **argv) {
    const char *driver;

    if (argc != 1 && argc != 2) {
        printf("Usage: %s [device]\r\n", argv[0]);
        return;
    }

    if (argc == 2) {
        driver = argv[1];
    }
    else {
        driver = device_auto_lookup(&accel_class);
        if (!driver) {
            return;
        }
    }

    printf("Connecting to accelerometer '%s'\r\n", driver);

    struct obj *o = device_get(driver);
    if (!o) {
        printf("Error: unable to find accelerometer.\r\n");
        return;
    }

    struct accel *a = to_accel(o);
    struct accel_ops *ops = (struct accel_ops *) o->ops;
    struct accel_data data;

    printf("q to quit, any other key to get data.\r\nunits in g's\r\n");

    while(1) {
        if(getc() == 'q') {
            device_put(o);
            return;
        }
        else {
            if (!ops->get_data(a, &data)) {
                printf("Roll: %f X: %f Y: %f Z: %f\r\n", atan2(data.z, data.y)*RAD_TO_DEG, data.x, data.y, data.z);
            }
            else {
                printf("Unable to read accelerometer.\r\n");
            }
        }
    }
}
DEFINE_APP(accel)
