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
#include <mm/mm.h>
#include <dev/resource.h>
#include <kernel/sched.h>
#include <kernel/fault.h>
#include <kernel/init.h>

#include <dev/arch.h>
#include <dev/hw/perfcounter.h>
#include <dev/hw/led.h>
#include <dev/hw/usart.h>
#include <dev/hw/usbdev.h>

const char banner[] = "Welcome to...\r\n"
                "\r\n"
                "88888888888      ,d8      ,ad8888ba,     ad88888ba   \r\n"
                "88             ,d888     d8\"\'    `\"8b   d8\"     \"8b  \r\n"
                "88           ,d8\" 88    d8\'        `8b  Y8,          \r\n"
                "88aaaaa    ,d8\"   88    88          88  `Y8aaaaa,    \r\n"
                "88\"\"\"\"\"  ,d8\"     88    88          88    `\"\"\"\"\"8b,  \r\n"
                "88       8888888888888  Y8,        ,8P          `8b  \r\n"
                "88                88     Y8a.    .a8P   Y8a     a8P  \r\n"
                "88                88      `\"Y8888Y\"\'     \"Y88888P\"   \r\n"
                "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n";

void os_start(void) __attribute__((section(".kernel")));

void os_start(void) {
    init_arch();
    do_early_initializers();

#ifdef CONFIG_PERFCOUNTER
    init_perfcounter();
#endif

    init_heap();

#ifdef CONFIG_HAVE_LED
    init_power_led();
#endif

#ifdef CONFIG_HAVE_USART
    init_usart();
#endif

#ifdef CONFIG_HAVE_USBDEV
    init_usbdev();
#endif

    do_core_initializers();

    printf("\r\n%s\r\n", banner);

    if (default_resources[stdout] != default_resources[stderr]) {
        fprintf(stderr, "\r\n%sStandard error terminal.\r\n", banner);
    }

    do_late_initializers();
    start_sched();
    panic_print("Task switching ended.");
}

/* TODO: Something nicer when an initializer fails... */
static void do_initializers(initializer_t *start, initializer_t *end) {
    initializer_t *curr = start;

    for(; curr < end; curr++) {
        if((**curr)())
            panic();
    }
}

void do_early_initializers(void) {
    do_initializers(&__begin_early_initializer, &__end_early_initializer);
}

void do_core_initializers(void) {
    do_initializers(&__begin_core_initializer, &__end_core_initializer);
}

void do_late_initializers(void) {
    do_initializers(&__begin_late_initializer, &__end_late_initializer);
}
