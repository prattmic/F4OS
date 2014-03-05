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
#include <dev/device.h>
#include <dev/baro.h>
#include <kernel/obj.h>
#include "device_lookup.h"
#include "app.h"

void baro(int argc, char **argv) {
    const char *driver;

    if (argc != 1 && argc != 2) {
        printf("Usage: %s [device]\r\n", argv[0]);
        return;
    }

    if (argc == 2) {
        driver = argv[1];
    }
    else {
        driver = device_auto_lookup(&baro_class);
        if (!driver) {
            return;
        }
    }

    printf("Connecting to barometer '%s'\r\n", driver);

    struct obj *o = device_get(driver);
    if (!o) {
        printf("Error: unable to find barometer.\r\n");
        return;
    }

    struct baro *baro = to_baro(o);
    struct baro_ops *ops = (struct baro_ops *) o->ops;
    struct baro_data data;
    uint8_t has_temp = ops->has_temp(baro);

    printf("Baro provides temp: %s\r\n\r\n", has_temp ? "Yes" : "No");

    printf("q to quit, any other key to get data.\r\nunits in g's\r\n");

    while(1) {
        if(getc() == 'q') {
            device_put(o);
            return;
        }
        else {
            if (!ops->get_data(baro, &data)) {
                if (has_temp) {
                    printf("Temp: %fC\t", data.temperature);
                }

                printf("Pressure: %fPa\r\n", data.pressure);
            }
            else {
                printf("Unable to read barometer.\r\n");
            }
        }
    }
}
DEFINE_APP(baro)
