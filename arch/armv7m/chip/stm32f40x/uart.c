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
#include <stdlib.h>
#include <arch/chip/clock.h>
#include <arch/chip/dma.h>
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

#define STM32F4_UART_BUFFER_SIZE    (250)

struct stm32f4_uart {
    /* One lock must be held to read ready, both must be held to write it */
    uint8_t ready;

    int periph_id;
    long clock; /* Peripheral clock */
    unsigned int baud;  /* Last requested baud */

    struct gpio *gpio[2];    /* Each SPI port uses 2 GPIOs */
    struct stm32f4_dma *rx_dma;
    struct stm32f4_dma *tx_dma;
    stm32f4_dma_handle_t rx_handle;
    stm32f4_dma_handle_t tx_handle;
    struct stm32f4_usart_regs *regs;

    uint8_t *rx_buffer;
    uint8_t *tx_buffer;
    uint8_t first_tx;   /* Bootstrap initial TX */
    uint32_t read_index;/* Index of last read data in rx_buffer */
    /* DMA has wrapped around to beginning of rx buffer, before read_index */
    uint8_t wrapped;

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
    struct stm32f4_dma_ops *rx_ops = port->rx_dma->obj.ops;
    struct stm32f4_dma_ops *tx_ops = port->tx_dma->obj.ops;

    struct stm32f4_dma_config rx_config = {
        .direction = STM32F4_DMA_DIR_PERIPH_TO_MEM,
        .memory_size = 1,
        .peripheral_size = 1,
        .memory_increment = 1,
        .peripheral_increment = 0,
        .circular = 1,
        .double_buffer = 0,
        .peripheral_addr = (uintptr_t) &port->regs->DR,
        .mem0_addr = (uintptr_t) port->rx_buffer,
        .mem1_addr = (uintptr_t) NULL,
    };

    struct stm32f4_dma_config tx_config = {
        .direction = STM32F4_DMA_DIR_MEM_TO_PERIPH,
        .memory_size = 1,
        .peripheral_size = 1,
        .memory_increment = 1,
        .peripheral_increment = 0,
        .circular = 0,
        .double_buffer = 0,
        .peripheral_addr = (uintptr_t) &port->regs->DR,
        .mem0_addr = (uintptr_t) port->tx_buffer,
        .mem1_addr = (uintptr_t) NULL,
    };

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

    /* Enable DMA */
    raw_mem_set_bits(&port->regs->CR3, USART_CR3_DMAT | USART_CR3_DMAR);

    /* Configure DMAs */
    ret = rx_ops->configure(port->rx_dma, port->rx_handle, &rx_config);
    if (ret) {
        return ret;
    }

    ret = tx_ops->configure(port->tx_dma, port->tx_handle, &tx_config);
    if (ret) {
        return ret;
    }

    /* Begin continuous receive transaction */
    ret = rx_ops->begin_transaction(port->rx_dma, port->rx_handle,
                                    STM32F4_UART_BUFFER_SIZE);
    if (ret) {
        return ret;
    }

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
    /* Release DMAs? */
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
    struct stm32f4_dma_ops *rx_ops;
    int items_remaining, dma_read, wrapped;
    int i, ret;

    if (!uart) {
        return -1;
    }

    port = uart->priv;
    rx_ops = port->rx_dma->obj.ops;

    acquire(&port->read_lock);

    if (!port->ready) {
        acquire(&port->write_lock);
        ret = stm32f4_uart_initialize(port);
        release(&port->write_lock);
        if (ret) {
            goto out;
        }
    }

    /*
     * DMA has wrapped around to the beginning of the buffer since
     * the last read if a transaction has completed.
     *
     * DMA may have been indicated a wrapped state in a previous read,
     * so port->wrapped is used to keep track.  It will be reset to
     * not wrapped when the reading rolls over as well.
     */
    wrapped = rx_ops->transaction_complete(port->rx_dma, port->rx_handle);
    if (wrapped < 0) {
        ret = wrapped;
        goto out;
    }

    /*
     * DMA was already wrapped at the last check, and has since wrapped
     * *again*.  We are hopelessly behind; start over; data is lost.
     */
    if (wrapped && port->wrapped) {
        port->read_index = 0;
        wrapped = port->wrapped = 0;
    }
    /* Wrapped state is new */
    else if (wrapped && !port->wrapped) {
        port->wrapped = wrapped;
    }

    items_remaining = rx_ops->items_remaining(port->rx_dma, port->rx_handle);
    if (items_remaining < 0) {
        ret = items_remaining;
        goto out;
    }

    /* DMA has written to this point */
    dma_read = STM32F4_UART_BUFFER_SIZE - items_remaining;

    /* We are caught up to everything the DMA has read */
    if (!port->wrapped && dma_read == port->read_index) {
        ret = 0;
        goto out;
    }
    /*
     * The DMA has not wrapped around, yet is somehow behind us.
     * Start over; data is lost.
     */
    else if (!port->wrapped && dma_read < port->read_index) {
        port->read_index = 0;
        port->wrapped = 0;
    }
    /*
     * The DMA has wrapped around, and is already ahead of us.
     * Start over; data is lost.
     */
    else if (port->wrapped && dma_read >= port->read_index) {
        port->read_index = 0;
        port->wrapped = 0;
    }

    for (i = 0; i < len && port->read_index != dma_read; i++) {
        buf[i] = port->rx_buffer[port->read_index++];

        /*
         * Roll over to beginning of ring buffer.
         * The DMA is now definitely ahead of us.
         */
        if (port->read_index >= STM32F4_UART_BUFFER_SIZE) {
            port->read_index = 0;
            port->wrapped = 0;
        }
    }

    ret = i;

out:
    release(&port->read_lock);
    return ret;
}

static int stm32f4_uart_write(struct uart *uart, char *buf, size_t len) {
    struct stm32f4_uart *port;
    struct stm32f4_dma_ops *tx_ops;
    int ret;
    size_t tx_size;

    if (!uart) {
        return -1;
    }

    port = uart->priv;
    tx_ops = port->tx_dma->obj.ops;

    acquire(&port->write_lock);

    if (!port->ready) {
        acquire(&port->read_lock);
        ret = stm32f4_uart_initialize(port);
        release(&port->read_lock);
        if (ret) {
            goto out;
        }
    }

    /*
     * Wait if previous not transaction complete.
     * However, there is no previous transaction the first time
     */
    if (port->first_tx) {
        port->first_tx = 0;
    }
    else if (!tx_ops->transaction_complete(port->tx_dma, port->tx_handle)) {
        ret = 0;
        goto out;
    }

    tx_size = len < STM32F4_UART_BUFFER_SIZE ? len : STM32F4_UART_BUFFER_SIZE;
    memcpy(port->tx_buffer, buf, tx_size);

    /* TX via DMA */
    ret = tx_ops->begin_transaction(port->tx_dma, port->tx_handle, tx_size);
    if (ret) {
        goto out;
    }

    ret = tx_size;

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
    port->first_tx = 1;
    port->read_index = 0;
    port->wrapped = 0;

    /* Allocate DMA addressable buffers */
    port->rx_buffer = malloc(STM32F4_UART_BUFFER_SIZE);
    if (!port->rx_buffer) {
        goto err_free_port;
    }

    port->tx_buffer = malloc(STM32F4_UART_BUFFER_SIZE);
    if (!port->tx_buffer) {
        goto err_free_rx_buffer;
    }

    /* Get the RX and TX DMAs */
    err = stm32f4_dma_allocate(blob, offset, "rx", &port->rx_dma,
                               &port->rx_handle);
    if (err) {
        goto err_free_tx_buffer;
    }

    err = stm32f4_dma_allocate(blob, offset, "tx", &port->tx_dma,
                               &port->tx_handle);
    if (err) {
        goto err_dealloc_rx_dma;
    }

    bus = rcc_peripheral_bus(port->periph_id);
    if (bus == STM32F4_UNKNOWN_BUS) {
        goto err_dealloc_tx_dma;
    }

    port->clock = rcc_bus_clock(bus);
    if (port->clock <= 0) {
        goto err_dealloc_tx_dma;
    }

    gpio_af = gpio_periph_to_alt_func(port->periph_id);
    if (gpio_af == STM32F4_GPIO_AF_UNKNOWN) {
        goto err_dealloc_tx_dma;
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

err_dealloc_tx_dma:
    stm32f4_dma_deallocate(port->tx_dma, port->tx_handle);
err_dealloc_rx_dma:
    stm32f4_dma_deallocate(port->rx_dma, port->rx_handle);
err_free_tx_buffer:
    free(port->tx_buffer);
err_free_rx_buffer:
    free(port->rx_buffer);
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
