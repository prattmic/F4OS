/*
 * Copyright (C) 2013 F4OS Authors
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
#include <dev/device.h>
#include <dev/mag.h>
#include <kernel/obj.h>
#include "app.h"

void mag(int argc, char **argv) {
    if (argc != 1) {
        printf("Usage: %s\r\n", argv[0]);
        return;
    }

    struct obj *o = device_get("hmc5883");
    if (!o) {
        printf("Error: unable to find magnetometer.\r\n");
        return;
    }

    struct mag *mag = to_mag(o);
    struct mag_ops *ops = (struct mag_ops *) o->ops;
    struct mag_data data;

    printf("q to quit, any other key to get data.\r\nunits in gauss\r\n");

    while(1) {
        if(getc() == 'q') {
            device_put(o);
            return;
        }
        else {
            if (!ops->get_data(mag, &data)) {
                printf("X: %f Y: %f Z: %f\r\n", data.x, data.y, data.z);
            }
            else {
                printf("Unable to read magnetometer.\r\n");
            }
        }
    }
}
DEFINE_APP(mag)
