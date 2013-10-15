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

#ifndef DEV_HW_LED_H_INCLUDED
#define DEV_HW_LED_H_INCLUDED

#include <stdint.h>
#include <kernel/obj.h>

struct led {
    uint32_t gpio;
    uint8_t active_low;
};

/**
 * Enable power indication LED
 *
 * If LED cannot be initialized, the function returns as normal
 */
void init_power_led(void) __attribute__((section(".kernel")));

/**
 * Power led object
 *
 * Used by fault handlers for indicating error conditions
 */
extern struct obj *power_led;

/**
 * Get LED object instance
 *
 * @param num   LED number to get instance of
 * @returns LED obj instance, NULL on failure
 */
struct obj *led_get(uint8_t num);

/**
 * Put LED object instance
 *
 * Return an LED object instance when done with it
 *
 * @param led   LED object to put
 */
void led_put(struct obj *led);

/**
 * Set LED output value
 *
 * @param led   LED object to set
 * @param value 1 to enable LED, 0 to disable LED
 * @returns 0 on success, negative on error
 */
int led_set(struct obj *led, uint8_t value);

/**
 * Enable LED
 *
 * @param led   LED object to enable
 * @returns zero on success, negative on error
 */
int led_enable(struct obj *led);

/**
 * Disable LED
 *
 * @param led   LED object to disable
 * @returns zero on success, negative on error
 */
int led_disable(struct obj *led);

/**
 * Toggle LED
 *
 * Switch LED from enabled to disabled or disable to enabled
 *
 * @param led   LED object to toggle
 * @returns zero on success, negative on error
 */
int led_toggle(struct obj *led);

/* Provided in a board-specific file */
extern const struct led leds_avail[];
extern const int num_leds;

#endif
