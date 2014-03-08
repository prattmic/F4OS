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

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dev/raw_mem.h>
#include <dev/resource.h>
#include <kernel/semaphore.h>
#include <kernel/fault.h>
#include "clocks.h"
#include "memory_map.h"

#define conf_uart0_rxd  ((volatile uint32_t *) (AM335X_CTL_MODULE_BASE + 0x970))
#define conf_uart0_txd  ((volatile uint32_t *) (AM335X_CTL_MODULE_BASE + 0x974))

#define UART0_RHR       ((volatile uint32_t *) (AM335X_UART0_BASE + 0x00))
#define UART0_THR       ((volatile uint32_t *) (AM335X_UART0_BASE + 0x00))
#define UART0_DLL       ((volatile uint32_t *) (AM335X_UART0_BASE + 0x00))
#define UART0_DLM       ((volatile uint32_t *) (AM335X_UART0_BASE + 0x04))
#define UART0_FCR       ((volatile uint32_t *) (AM335X_UART0_BASE + 0x08))
#define UART0_LCR       ((volatile uint32_t *) (AM335X_UART0_BASE + 0x0C))
#define UART0_LSR       ((volatile uint32_t *) (AM335X_UART0_BASE + 0x14))
#define UART0_MDR1      ((volatile uint32_t *) (AM335X_UART0_BASE + 0x20))
#define UART0_SYSC      ((volatile uint32_t *) (AM335X_UART0_BASE + 0x40))
#define UART0_SYSS      ((volatile uint32_t *) (AM335X_UART0_BASE + 0x44))

struct semaphore am335x_usart_read_semaphore = INIT_SEMAPHORE;
struct semaphore am335x_usart_write_semaphore = INIT_SEMAPHORE;

int usart_ready = 1;

void init_usart(void) {
    struct am335x_clock_cmwkup *cmwkup =
        (struct am335x_clock_cmwkup *) AM335X_CM_WKUP_BASE;

    /* Enable control module */
    raw_mem_set_mask(&cmwkup->wkup_control_clkctrl, CM_MODULEMODE,
                     CM_MODULEMODE_ENABLED);
    while(!(raw_mem_read(&cmwkup->wkup_control_clkctrl) & CM_MODULEMODE_ENABLED));

    /* Enable UART0 module */
    raw_mem_set_mask(&cmwkup->wkup_uart0_clkctrl, CM_MODULEMODE,
                     CM_MODULEMODE_ENABLED);
    while(!(raw_mem_read(&cmwkup->wkup_uart0_clkctrl) & CM_MODULEMODE_ENABLED));

    /* RX pin receive active, pull up enable */
    raw_mem_write(conf_uart0_rxd, (1 << 4) | (1 << 5));

    /* TX pin pullup/down enable */
    raw_mem_write(conf_uart0_txd, (1 << 3));

    /* Reset UART */
    raw_mem_write(UART0_SYSC, (1 << 1));
    while(!raw_mem_read(UART0_SYSS));

    /* Switch to divisors, 8N1 */
    raw_mem_write(UART0_LCR, (1 << 7) | 0x3);

    /* Zero divisors */
    raw_mem_write(UART0_DLL, 0);
    raw_mem_write(UART0_DLM, 0);

    /* Switch from divisors, 8N1 */
    raw_mem_write(UART0_LCR, 0x3);

    /* Enable and clear FIFOs */
    raw_mem_write(UART0_FCR, 0x7);

    /* Switch to divisors, 8N1 */
    raw_mem_write(UART0_LCR, (1 << 7) | 0x3);

    uint32_t divisor = (48000000/16) / 115200;

    /* Set divisors */
    raw_mem_write(UART0_DLL, divisor & 0xff);
    raw_mem_write(UART0_DLM, (divisor >> 8) & 0xff);

    /* Switch from divisors, 8N1 */
    raw_mem_write(UART0_LCR, 0x3);

    /* UART 16x mode */
    raw_mem_write(UART0_MDR1, 0);

    usart_ready = 1;
}

int usart_putc(char c, void *env) {
    /* Wait for space in FIFO */
    while (!(raw_mem_read(UART0_LSR) & (1 << 5)));

    raw_mem_write(UART0_THR, c);

    return 1;
}

int usart_puts(char *s, void *env) {
    int total = 0;
    while (*s) {
        int ret = usart_putc(*s++, env);
        if (ret > 0) {
            total += ret;
        }
        else {
            total = ret;
            break;
        }
    }

    return total;
}

char usart_getc(void *env, int *error) {
    if (error != NULL) {
        *error = 0;
    }

    /* Wait for data */
    while (!(raw_mem_read(UART0_LSR) & 0x1));

    return raw_mem_read(UART0_RHR) & 0xff;
}

int usart_close(resource *resource) {
    printk("OOPS: USART is a fundamental resource, it may not be closed.");
    return -1;
}

resource usart_resource = { .writer     = &usart_putc,
                            .swriter    = &usart_puts,
                            .reader     = &usart_getc,
                            .closer     = &usart_close,
                            .env        = NULL,
                            .read_sem   = &am335x_usart_read_semaphore,
                            .write_sem  = &am335x_usart_write_semaphore};

