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
#include <dev/rotary_encoder.h>
#include <kernel/obj.h>
#include "app.h"

void rotary_encoder(int argc, char **argv) {
    if (argc != 1) {
        printf("Usage: %s\r\n", argv[0]);
        return;
    }

    struct obj *o = device_get("as5048b");
    if (!o) {
        printf("Error: unable to find rotary encoder.\r\n");
        return;
    }

    struct rotary_encoder *rotary_encoder = to_rotary_encoder(o);
    struct rotary_encoder_ops *ops = (struct rotary_encoder_ops *) o->ops;
    struct rotary_encoder_data data;

    printf("q to quit, any other key to get data.\r\nunits in radians\r\n");

    while(1) {
        if(getc() == 'q') {
            device_put(o);
            return;
        }
        else {
            if (!ops->get_data(rotary_encoder, &data)) {
                printf("Angle: %frads\r\n", data.angle);
            }
            else {
                printf("Unable to read rotary encoder.\r\n");
            }
        }
    }
}
DEFINE_APP(rotary_encoder)
