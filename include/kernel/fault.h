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

#ifndef KERNEL_FAULT_H_INCLUDED
#define KERNEL_FAULT_H_INCLUDED

#ifdef DEBUG
#define DEBUG_ON    1
#else
#define DEBUG_ON    0
#endif

#define DEBUG_PRINT(...)    do { if (DEBUG_ON) printk(__VA_ARGS__); } while(0)

extern void panic(void) __attribute__((noreturn));
extern void disable_interrupts(void);

int printk(char *fmt, ...) __attribute__((section(".kernel")));
void panic_print(char *fmt, ...) __attribute__((noreturn));
void toggle_led_delay(void) __attribute__((optimize(0)));

/**
 * Print a warning when a condition is true
 *
 * When _cond is true, output a warning with basic debugging information
 * with printk().
 */
#define WARN_ON(_cond)  do {    \
    if (_cond) {   \
        printk("WARNING: assertion '%s' is false\r\n", STRINGIFY((_cond))); \
        printk("note: in %s() at %s:%d\r\n", __func__, __FILE__, __LINE__);   \
    }   \
} while (0)

/**
 * Print an error and panic when a condition is true
 *
 * When _cond is true, output an error with basic debugging information
 * with printk(), then panic the system.
 */
#define PANIC_ON(_cond)  do {    \
    if (_cond) {   \
        printk("ERROR: assertion '%s' is false\r\n", STRINGIFY((_cond))); \
        printk("note: in %s() at %s:%d\r\n", __func__, __FILE__, __LINE__);   \
        panic();    \
    }   \
} while (0)

#endif
