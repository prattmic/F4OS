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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dev/hw/led.h>
#include <kernel/sched.h>
#include <kernel/obj.h>
#include "app.h"

void led1(void);
void led2(void);
void led3(void);

struct blinker {
    char *name;
    void (*func)(void);
    uint8_t enabled;
} blink_funcs[] = {
    {.name = "1", .func = &led1, .enabled = 0},
    {.name = "2", .func = &led2, .enabled = 0},
    {.name = "3", .func = &led3, .enabled = 0}
};

void blink(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s LED...\r\nExample: blink 2 1\r\n", argv[0]);
        return;
    }

    uint8_t found = 0;

    for (int i = 1; i < argc; i++) {
        for (int j = 0; j < num_leds - 1; j++) {
            if (!strncmp(argv[i], blink_funcs[j].name, 16)) {
                if (blink_funcs[j].enabled) {
                    printf("LED %s already enabled.\r\n", blink_funcs[j].name);
                }
                else {
                    printf("Enabling LED %s...", blink_funcs[j].name);
                    new_task(blink_funcs[j].func, 5, 200*(j+1));
                    blink_funcs[j].enabled = 1;
                    printf("Done.\r\n");
                }

                found = 1;
                continue;
            }
        }

        if (!found) {
            printf("Unknown LED: %s\r\n", argv[i]);
        }
    }
}
DEFINE_APP(blink)

void led1(void) {
    static struct obj *led = NULL;
    if (!led) {
        led = led_get(1);
        if (!led) {
            printf("Failed to get LED 1, aborting\r\n");
            abort();
        }
    }

    led_toggle(led);
}

void led2(void) {
    static struct obj *led = NULL;
    if (!led) {
        led = led_get(2);
        if (!led) {
            printf("Failed to get LED 2, aborting\r\n");
            abort();
        }
    }

    led_toggle(led);
}

void led3(void) {
    static struct obj *led = NULL;
    if (!led) {
        led = led_get(3);
        if (!led) {
            printf("Failed to get LED 3, aborting\r\n");
            abort();
        }
    }

    led_toggle(led);
}
