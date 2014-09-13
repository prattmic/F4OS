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

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <compiler.h>
#include <dev/char.h>
#include <dev/hw/usart.h>
#include <dev/hw/led.h>
#include <kernel/sched.h>
#include <kernel/mutex.h>

#include <kernel/fault.h>

#ifdef CONFIG_HAVE_USART
/*
 * These puts/putc ignore any locking on the resource.
 */
static int printk_puts(struct char_device *dev, char *s) {
    if (usart_ready) {
        return usart_puts(s, NULL);
    }
    else {
        return -1;
    }
}

static int printk_putc(struct char_device *dev, char c) {
    if (usart_ready) {
        return usart_putc(c, NULL);
    }
    else {
        return -1;
    }
}
#else
static int printk_puts(struct char_device *dev, char *s) {return -1;}
static int printk_putc(struct char_device *dev, char c) {return -1;}
#endif

static inline int vprintk(char *fmt, va_list ap) {
    return vfprintf(stderr, fmt, ap, &printk_puts, &printk_putc);
}

int printk(char *fmt, ...) {
    int ret;
    va_list ap;

    va_start(ap, fmt);
    ret = vprintk(fmt, ap);
    va_end(ap);

    return ret;
}

/* Print a message and then panic
 * Accepts standard printf format strings. */
void panic_print(char *fmt, ...) {
    /* Disable interrupts, as the system is going down. */
    disable_interrupts();

#ifdef CONFIG_HAVE_LED
    /* Toggle red LED so there is some indication that something
     * bad has happened if this hangs */
    if (power_led) {
        led_toggle(power_led);
    }
#endif

    /* The system is going to panic, so go ahead and end task switching */
    task_switching = 0;

    /* Print panic message */
    printk("\r\npanic: ");

    va_list ap;
    va_start(ap, fmt);
    vprintk(fmt, ap);
    va_end(ap);

    panic();
}

#ifdef CONFIG_HAVE_LED
void toggle_led_delay(void) {
    uint32_t count = 3000000;

    /* Toggle LED */
    if (power_led) {
        led_toggle(power_led);
    }

    while (--count);
}
#endif

/*
 * Provide a weak version that does nothing in case
 * the arch doesn't provide one
 */
void __weak disable_interrupts(void) {}
