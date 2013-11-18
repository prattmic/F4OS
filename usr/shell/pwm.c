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

#include <stdlib.h>
#include <stdio.h>
#include <arch/chip/gpio.h>
#include <dev/hw/gpio.h>
#include <dev/hw/pwm.h>
#include <kernel/obj.h>
#include "app.h"

static char *usage =
    "usage: pwm gpio_num duty_cycle pulse_width\r\n\r\n"
    "Enable PWM on gpio with given duty cycle and pulse width\r\n"
    "Once the output is enabled, there is currently no way to disable\r\n"
    "or modify the output.\r\n";

/* Abandons the PWM obj, making disabling or modifying the PWM impossible! */
void pwm(int argc, char **argv) {
    struct obj *pwm_obj;
    struct pwm *pwm;
    struct pwm_ops *ops;
    struct obj *gpio;
    int gpio_num, duty, width;
    int ret;

    if (argc != 4) {
        puts(usage);
        return;
    }

    gpio_num = atoi(argv[1]);
    duty = atoi(argv[2]);
    width = atoi(argv[3]);

    gpio = gpio_get(gpio_num);
    if (!gpio) {
        printf("Unable to get GPIO\r\n");
        return;
    }

    pwm_obj = pwm_get(gpio, duty);
    if (!pwm_obj) {
        printf("Unable to get PWM\r\n");
        return;
    }

    gpio_put(gpio);

    ops = pwm_obj->ops;
    pwm = to_pwm(pwm_obj);

    ret = ops->set_pulse_width(pwm, width);

    ret = ops->enable(pwm, 1);
    if (ret) {
        printf("Unable to enable PWM\r\n");
        return;
    }
}
DEFINE_APP(pwm);
