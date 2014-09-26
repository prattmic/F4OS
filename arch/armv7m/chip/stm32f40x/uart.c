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

#include <libfdt.h>
#include <arch/chip/clock.h>
#include <arch/chip/gpio.h>
#include <arch/chip/rcc.h>
#include <arch/chip/usart.h>
#include <dev/device.h>
#include <dev/fdtparse.h>
#include <dev/hw/gpio.h>
#include <dev/hw/uart.h>
#include <dev/raw_mem.h>
#include <kernel/class.h>
#include <kernel/init.h>
#include <kernel/mutex.h>
#include <mm/mm.h>

#define STM32F4_UART_COMPAT "stmicro,stm32f407-uart"

struct stm32f4_uart {
    /* One lock must be held to read ready, both must be held to write it */
    uint8_t ready;
    int periph_id;
    long clock; /* Peripheral clock */
    unsigned int baud;  /* Last requested baud */
    struct gpio *gpio[2];    /* Each SPI port uses 2 GPIOs */
    struct stm32f4_usart_regs *regs;
    struct mutex read_lock;
    struct mutex write_lock;
};

static uint16_t baud_to_brr(long clock, uint32_t baud) {
    uint32_t divisor = 16 * baud;
    uint16_t mantissa = clock / divisor;
    uint32_t remainder = clock % divisor;
    uint16_t fraction = (16 * remainder) / divisor;

    return (mantissa << 4) | (fraction & 0xf);
}

static uint32_t brr_to_baud(long clock, uint32_t brr) {
    uint16_t mantissa = (brr >> 4) & 0xfff;
    uint16_t fraction = brr & 0xf;

    return clock / (16*mantissa + fraction);
}

/*
 * Peripheral initialization.
 * Read and write locks must be held when calling,
 */
static int stm32f4_uart_initialize(struct stm32f4_uart *port) {
    int ret = 0;

    /* Enable clock */
    ret = rcc_set_clock_enable(port->periph_id, 1);
    if (ret) {
        return ret;
    }

    /* Enable peripheral */
    raw_mem_set_bits(&port->regs->CR1, USART_CR1_UE);

    /* 1 start bit, 8 data bits */
    raw_mem_clear_bits(&port->regs->CR1, USART_CR1_M);

    /* 1 stop bit */
    raw_mem_set_mask(&port->regs->CR2, USART_CR2_STOP_MASK,
                     USART_CR2_STOP_1BIT);

    /* Set baud rate */
    raw_mem_write(&port->regs->BRR, baud_to_brr(port->clock, port->baud));

    /* Enable reciever and transmitter */
    raw_mem_set_bits(&port->regs->CR1, USART_CR1_RE | USART_CR1_TE);

    port->ready = 1;

    return 0;
}

static int stm32f4_uart_init(struct uart *uart) {
    struct stm32f4_uart *port;
    int ret = 0;

    if (!uart) {
        return -1;
    }

    port = uart->priv;

    acquire(&port->read_lock);
    acquire(&port->write_lock);

    ret = stm32f4_uart_initialize(port);

    release(&port->write_lock);
    release(&port->read_lock);
    return ret;
}

static int stm32f4_uart_deinit(struct uart *s) {
    /* Turn off clocks? */
    /* Release GPIOs? */

    return 0;
}

static int stm32f4_uart_get_baud_rate(struct uart *uart) {
    struct stm32f4_uart *port;
    uint32_t brr;
    int ret;

    if (!uart) {
        return -1;
    }

    port = uart->priv;

    acquire(&port->read_lock);

    if (!port->ready) {
        acquire(&port->write_lock);
        ret = stm32f4_uart_initialize(port);
        release(&port->write_lock);
        if (ret) {
            goto out;
        }
    }

    brr = raw_mem_read(&port->regs->BRR);

    ret = brr_to_baud(port->clock, brr);

out:
    release(&port->read_lock);

    return ret;
}

static int stm32f4_uart_set_baud_rate(struct uart *uart, unsigned int baud) {
    struct stm32f4_uart *port;
    uint32_t brr;
    int ret;

    if (!uart) {
        return -1;
    }

    port = uart->priv;

    brr = baud_to_brr(port->clock, baud);

    acquire(&port->read_lock);
    acquire(&port->write_lock);

    if (!port->ready) {
        ret = stm32f4_uart_initialize(port);
        if (ret) {
            goto out;
        }
    }

    /* Remember desired baud */
    port->baud = baud;

    raw_mem_write(&port->regs->BRR, brr);

    ret = brr_to_baud(port->clock, brr);

out:
    release(&port->read_lock);
    release(&port->write_lock);

    return ret;
}

static int stm32f4_uart_read(struct uart *uart, char *buf, size_t len) {
    struct stm32f4_uart *port;
    int ret, read = 0;

    if (!uart) {
        return -1;
    }

    port = uart->priv;

    acquire(&port->read_lock);

    if (!port->ready) {
        acquire(&port->write_lock);
        ret = stm32f4_uart_initialize(port);
        release(&port->write_lock);
        if (ret) {
            goto out;
        }
    }

    for (int i = 0; i < len; i++) {
        /* Data available? */
        if (!(raw_mem_read(&port->regs->SR) & USART_SR_RXNE)) {
            break;
        }

        buf[i] = raw_mem_read(&port->regs->DR);
        read++;
    }

    ret = read;

out:
    release(&port->read_lock);
    return ret;
}

static int stm32f4_uart_write(struct uart *uart, char *buf, size_t len) {
    struct stm32f4_uart *port;
    int ret, written = 0;

    if (!uart) {
        return -1;
    }

    port = uart->priv;

    acquire(&port->write_lock);

    if (!port->ready) {
        acquire(&port->read_lock);
        ret = stm32f4_uart_initialize(port);
        release(&port->read_lock);
        if (ret) {
            goto out;
        }
    }

    for (int i = 0; i < len; i++) {
        /* Space available? */
        if (!(raw_mem_read(&port->regs->SR) & USART_SR_TXE)) {
            break;
        }

        raw_mem_write(&port->regs->DR, buf[i]);
        written++;
    }

    ret = written;

out:
    release(&port->write_lock);
    return ret;
}

static struct uart_ops stm32f4_uart_ops = {
    .init = stm32f4_uart_init,
    .deinit = stm32f4_uart_deinit,
    .get_baud_rate = stm32f4_uart_get_baud_rate,
    .set_baud_rate = stm32f4_uart_set_baud_rate,
    .read = stm32f4_uart_read,
    .write = stm32f4_uart_write,
};

static int stm32f4_uart_probe(const char *name) {
    const void *blob = fdtparse_get_blob();
    int offset;

    /* Lookup peripheral node */
    offset = fdt_path_offset(blob, name);
    if (offset < 0) {
        return 0;
    }

    /* Check that peripheral is compatible with driver */
    return fdt_node_check_compatible(blob, offset, STM32F4_UART_COMPAT) == 0;
}

static struct obj *stm32f4_uart_ctor(const char *name) {
    const void *blob = fdtparse_get_blob();
    int offset, err, periph_id, gpio_af;
    struct obj *obj;
    struct uart *uart;
    struct stm32f4_usart_regs *regs;
    struct stm32f4_uart *port;
    enum stm32f4_bus bus;

    offset = fdt_path_offset(blob, name);
    if (offset < 0) {
        return NULL;
    }

    if (fdt_node_check_compatible(blob, offset, STM32F4_UART_COMPAT)) {
        return NULL;
    }

    regs = fdtparse_get_addr32(blob, offset, "reg");
    if (!regs) {
        return NULL;
    }

    err = fdtparse_get_int(blob, offset, "stmicro,periph-id", &periph_id);
    if (err) {
        return NULL;
    }

    obj = instantiate(name, &uart_class, &stm32f4_uart_ops, struct uart);
    if (!obj) {
        return NULL;
    }

    uart = to_uart(obj);

    uart->priv = kmalloc(sizeof(struct stm32f4_uart));
    if (!uart->priv) {
        goto err_free_obj;
    }

    port = uart->priv;
    memset(port, 0, sizeof(*port));

    port->ready = 0;
    port->baud = 115200;    /* Default baud */
    port->periph_id = periph_id;
    port->regs = regs;
    init_mutex(&port->read_lock);
    init_mutex(&port->write_lock);

    bus = rcc_peripheral_bus(port->periph_id);
    if (bus == STM32F4_UNKNOWN_BUS) {
        goto err_free_port;
    }

    port->clock = rcc_bus_clock(bus);
    if (port->clock <= 0) {
        goto err_free_port;
    }

    gpio_af = gpio_periph_to_alt_func(port->periph_id);
    if (gpio_af == STM32F4_GPIO_AF_UNKNOWN) {
        goto err_free_port;
    }

    /* Setup GPIOs */
    for (int i = 0; i < 2; i++) {
        const char *i2c_gpio_props[2] =
            {"uart,tx-gpio", "uart,rx-gpio"};
        struct fdt_gpio fdt_gpio;
        struct obj *gpio_obj;
        struct gpio *gpio;
        struct gpio_ops *ops;
        int err;

        err = fdtparse_get_gpio(blob, offset, i2c_gpio_props[i], &fdt_gpio);
        if (err) {
            goto err_free_gpio;
        }

        gpio_obj = gpio_get(fdt_gpio.gpio);
        if (!gpio_obj) {
            goto err_free_gpio;
        }

        gpio = to_gpio(gpio_obj);

        ops = gpio_obj->ops;

        /* High speed GPIOs */
        err = ops->set_flags(gpio, STM32F4_GPIO_SPEED,
                             STM32F4_GPIO_SPEED_50MHZ);
        if (err) {
            goto err_free_gpio;
        }

        /* Push-pull GPIOs */
        err = ops->set_flags(gpio, STM32F4_GPIO_TYPE,
                             STM32F4_GPIO_PUSH_PULL);
        if (err) {
            goto err_free_gpio;
        }

        /* No pull up or down */
        err = ops->set_flags(gpio, STM32F4_GPIO_PULL,
                             STM32F4_GPIO_PULL_NONE);
        if (err) {
            goto err_free_gpio;
        }

        /* Set to proper UART mode */
        err = ops->set_flags(gpio, STM32F4_GPIO_ALT_FUNC,
                             gpio_af);
        if (err) {
            goto err_free_gpio;
        }

        port->gpio[i] = gpio;
    }

    /* Export to the OS */
    class_export_member(obj);

    return obj;

err_free_gpio:
    for (int i = 0; i < 2; i++) {
        if (port->gpio[i]) {
            gpio_put(&port->gpio[i]->obj);
        }
    }

err_free_port:
    kfree(port);

err_free_obj:
    class_deinstantiate(obj);

    return NULL;
}

static struct mutex stm32f4_uart_driver_mut = INIT_MUTEX;

static struct device_driver stm32f4_uart_compat_driver = {
    .name = STM32F4_UART_COMPAT,
    .probe = stm32f4_uart_probe,
    .ctor = stm32f4_uart_ctor,
    .class = &uart_class,
    .mut = &stm32f4_uart_driver_mut,
};

static int stm32f4_uart_register(void) {
    device_compat_driver_register(&stm32f4_uart_compat_driver);
    return 0;
}
CORE_INITIALIZER(stm32f4_uart_register)
