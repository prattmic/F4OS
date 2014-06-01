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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dev/hw/gpio.h>
#include <dev/hw/adc.h>
#include <kernel/obj.h>
#include "app.h"

struct adc *get_adc(int gpio_num) {
    struct obj *adc_obj;
    struct obj *gpio;

    gpio = gpio_get(gpio_num);
    if (!gpio) {
        printf("Unable to get GPIO %d\r\n", gpio_num);
        return NULL;
    }

    adc_obj = adc_get(gpio, NULL);
    if (!adc_obj) {
        printf("Unable to get ADC from GPIO %d\r\n", gpio_num);
        gpio_put(gpio);
        return NULL;
    }

    gpio_put(gpio);

    return to_adc(adc_obj);
}

void adc(int argc, char **argv) {
    if (argc < 2) {
        printf("%s gpio_num1 [gpio_num2 ...]\r\n", argv[0]);
        return;
    }

    argv++;
    argc--;

    struct adc *adc[argc];
    memset(adc, 0, sizeof(adc));

    for (int i = 0; i < argc; i++) {
        int gpio_num = atoi(argv[i]);
        adc[i] = get_adc(gpio_num);
        if (!adc[i]) {
            goto out;
        }
    }

    printf("Press any key to read, 'q' to quit\r\n");

    while (getc() != 'q') {
        for (int i = 0; i < argc; i++) {
            struct adc_ops *ops = (struct adc_ops *) adc[i]->obj.ops;
            uint32_t val = ops->read_raw(adc[i]);
            printf("%u\t", val);
        }
        printf("\r\n");
    }

out:
    for (int i = 0; i < argc; i++) {
        if (adc[i]) {
            adc_put(&adc[i]->obj);
        }
    }
}
DEFINE_APP(adc);
