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

#include <libfdt.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <arch/chip/gpio.h>
#include <arch/chip/i2c.h>
#include <arch/chip/rcc.h>
#include <dev/device.h>
#include <dev/fdtparse.h>
#include <dev/hw/gpio.h>
#include <dev/raw_mem.h>
#include <dev/resource.h>
#include <kernel/fault.h>
#include <kernel/mutex.h>
#include <kernel/class.h>
#include <kernel/init.h>
#include <mm/mm.h>

#include <dev/hw/i2c.h>

#define STM32F4_I2C_COMPAT "stmicro,stm32f407-i2c"

enum {
    I2C_GPIO_SCL,
    I2C_GPIO_SDA,
};

struct stm32f4_i2c {
    uint8_t                 ready;
    int                     periph_id;
    struct gpio             *gpio[2];   /* SCL, then SDA */
    struct stm32f4_i2c_regs *regs;
};

/*
 * I2C peripheral initialization
 *
 * Initialize the I2C peripheral registers to the standard state.
 * For now, all I2C ports are configured the same.
 *
 * The I2C mutex should be held when calling this function.
 *
 * @param i2c   I2C peripheral to initialize
 * @returns zero on success, negative on error
 */
static int stm32f4_i2c_initialize(struct i2c *i2c) {
    struct stm32f4_i2c *port = i2c->priv;

    /* Enable I2C clock */
    if (rcc_set_clock_enable(port->periph_id, 1)) {
        return -1;
    }

    /* Configure peripheral */
    raw_mem_set_mask(&port->regs->CR2, I2C_CR2_FREQ_MASK, I2C_CR2_FREQ(42));

    /* Set clock to 300kHz */
    raw_mem_set_mask(&port->regs->CCR, I2C_CCR_CCR_MASK, I2C_CCR_CCR(140));
    raw_mem_write(&port->regs->TRISE, 43);

    /* Enable */
    raw_mem_set_bits(&port->regs->CR1, I2C_CR1_PE);

    port->ready = 1;

    return 0;
}

/*
 * For the init() method, just grab the lock and call the actual initialization
 * function.
 *
 * If already initialized, even that can be skipped.
 */
static int stm32f4_i2c_init(struct i2c *i2c) {
    struct stm32f4_i2c *port = i2c->priv;
    int ret = 0;

    acquire(&i2c->lock);

    /* Already initialized? */
    if (port->ready) {
        goto out;
    }

    ret = stm32f4_i2c_initialize(i2c);

out:
    release(&i2c->lock);

    return ret;
}

static int stm32f4_i2c_deinit(struct i2c *i2c) {
    /* Turn off clocks? */
    /* Release GPIOs? */

    return 0;
}

/**
 * Reset I2C port
 *
 * In the event of a bus error (start or stop detected during data transfer),
 * the peripheral continues as normal, but the slave has likely cancelled
 * communication, leaving the peripheral expecting communication that won't
 * occur.  The only solution is to reset the peripheral.
 *
 * The I2C mutex should be held when calling this function.
 *
 * Upon successful return, normal I2C use can continue.
 *
 * @param i2c   I2C port to rest
 * @returns 0 on successful reset, negative otherwiese
 */
static int stm32f4_i2c_reset(struct i2c *i2c) {
    struct stm32f4_i2c *port = i2c->priv;

    port->ready = 0;

    /* Software reset */
    raw_mem_set_bits(&port->regs->CR1, I2C_CR1_SWRST);

    /* Wait until peripheral is disabled */
    while (raw_mem_read(&port->regs->CR1) & I2C_CR1_PE);

    /* Re-enable */
    raw_mem_clear_bits(&port->regs->CR1, I2C_CR1_SWRST);

    /* Re-initialize */
    return stm32f4_i2c_initialize(i2c);
}

/**
 * Force clear of BUSY bit
 *
 * On rare occassions, the I2C device will get confused, either because we
 * missed a timing requirement, or it is just stupid.  Regardless, it holds
 * SDA low waiting for some unknown action from the master.  This keeps the
 * bus BUSY and prevents any further communication.  This condition is fixed
 * by manually clocking SCL until SDA is released by the slave.  As far as it
 * is concerned, we just completed a normal transaction.
 *
 * The I2C mutex should be held when calling this function.
 *
 * @param i2c   I2C bus to clear
 * @returns 0 if BUSY successfully cleared, negative otherwise
 */
static int stm32f4_i2c_force_clear_busy(struct i2c *i2c) {
    struct stm32f4_i2c *port = i2c->priv;
    struct gpio *scl = port->gpio[I2C_GPIO_SCL];
    struct gpio *sda = port->gpio[I2C_GPIO_SDA];
    struct gpio_ops *scl_ops = (struct gpio_ops *) scl->obj.ops;
    struct gpio_ops *sda_ops = (struct gpio_ops *) sda->obj.ops;
    int count = 10000;

    /* Set pins to output/input */
    scl_ops->direction(scl, GPIO_OUTPUT);
    sda_ops->direction(sda, GPIO_INPUT);

    /* Toggle clock until bus no longer busy */
    while (!(sda_ops->get_input_value(sda))) {
        if (!count--) {
            /* Out of time, perhaps the last ditch effort will save us. */
            break;
        }

        /* Toggle clock */
        scl_ops->set_output_value(scl, !scl_ops->get_output_value(scl));
        for (volatile int delay = 100; delay > 0; delay--);
    }

    /* Back to I2C mode */
    scl_ops->set_flags(scl, STM32F4_GPIO_ALT_FUNC, STM32F4_GPIO_AF_I2C);
    sda_ops->set_flags(sda, STM32F4_GPIO_ALT_FUNC, STM32F4_GPIO_AF_I2C);
    for (volatile int delay = 100; delay > 0; delay--);

    /* Make sure the peripheral recognizes that the bus is now free */
    if (raw_mem_read(&port->regs->SR2) & I2C_SR2_BUSY) {
        /* Last ditch effort */
        if (stm32f4_i2c_reset(i2c) ||
            (raw_mem_read(&port->regs->SR2) & I2C_SR2_BUSY)) {
            /* Failed to reset */
            printk("I2C: BUSY flag failed to clear.\r\n");
            printk("I2C: Bus unrecoverable, device must be reset.\r\n");
            return -1;
        }
    }

    return 0;
}

/**
 * Prepare I2C bus for transaction
 *
 * Ensures the bus is ready and starts a transaction.
 *
 * This function acquires the I2C bus lock, which must be released
 * when the transaction is complete.
 *
 * @param i2c   I2C bus to prepare
 * @returns 0 if ready, negative on error
 */
static int stm32f4_i2c_prepare(struct i2c *i2c) {
    struct stm32f4_i2c *port;
    int ret, count;

    if (!i2c) {
        return -1;
    }

    acquire(&i2c->lock);

    port = i2c->priv;
    if (!port->ready) {
        ret = stm32f4_i2c_initialize(i2c);
        if (ret) {
            return ret;
        }
    }

    /* Check for bus error */
    if (raw_mem_read(&port->regs->SR1) & I2C_SR1_BERR) {
        printk("I2C: Bus error, reseting.\r\n");
        /* Clear the error and reset I2C */
        raw_mem_clear_bits(&port->regs->SR1, I2C_SR1_BERR);

        ret = stm32f4_i2c_reset(i2c);
        if (ret) {
            /* Failed to reset */
            goto err_release;
        }
    }

    /* Wait until BUSY is reset and previous transaction STOP is complete */
    count = 10000;
    while ((raw_mem_read(&port->regs->SR2) & I2C_SR2_BUSY) ||
            (raw_mem_read(&port->regs->CR1) & I2C_CR1_STOP)) {
        if (--count == 0) {
            printk("I2C: Stalled, reseting.\r\n");

            ret = stm32f4_i2c_reset(i2c);
            if (ret) {
                /* Failed to reset */
                goto err_release;
            }
        }
        else if (count < 0) {
            printk("I2C: Stalled, reset failed, force clearing busy.\r\n");

            ret = stm32f4_i2c_force_clear_busy(i2c);
            if (ret) {
                /* Failed to clear */
                goto err_release;
            }
        }
    }

    raw_mem_set_bits(&port->regs->CR1, I2C_CR1_START);

    count = 10000;
    while (!(raw_mem_read(&port->regs->SR1) & I2C_SR1_SB)) {
        if (!count--) {
            /* Start bit timeout */
            ret = -1;
            goto err_stop;
        }
    }

    return 0;

err_stop:
    raw_mem_set_bits(&port->regs->CR1, I2C_CR1_STOP);
err_release:
    release(&i2c->lock);
    return ret;
}

static int stm32f4_i2c_write(struct i2c *i2c, uint8_t addr, uint8_t *data,
                             uint32_t num) {
    struct stm32f4_i2c *port;
    int ret, count;

    if (!data || !num) {
        return -1;
    }

    ret = stm32f4_i2c_prepare(i2c);
    if (ret) {
        return ret;
    }

    port = i2c->priv;

    raw_mem_write(&port->regs->DR, addr << 1);

    count = 10000;
    while (!(raw_mem_read(&port->regs->SR1) & I2C_SR1_ADDR)) {
        if ((raw_mem_read(&port->regs->SR1) & I2C_SR1_AF) || !count--) {
            /* Clear error */
            raw_mem_clear_bits(&port->regs->SR1, I2C_SR1_AF);
            ret = -1;
            goto out;
        }
    }

    count = 10000;
    while (!(raw_mem_read(&port->regs->SR2) & I2C_SR2_MSL)) {
        if (!count--) {
            ret = -1;
            goto out;
        }
    }

    int total = 0;

    while (num--) {
        /* Make sure shift register is empty */
        count = 10000;
        while (!(raw_mem_read(&port->regs->SR1) & I2C_SR1_TXE)) {
            if (!count--) {
                ret = total;
                goto out;
            }
        }

        raw_mem_write(&port->regs->DR, *data++);

        count = 10000;
        while (!(raw_mem_read(&port->regs->SR1) & I2C_SR1_TXE)) {
            if (!count--) {
                ret = total;
                goto out;
            }
        }

        total += 1;
    }

    ret = total;

out:
    raw_mem_set_bits(&port->regs->CR1, I2C_CR1_STOP);
    release(&i2c->lock);

    return ret;
}

static int stm32f4_i2c_read(struct i2c *i2c, uint8_t addr, uint8_t *data,
                            uint32_t num) {
    struct stm32f4_i2c *port;
    int ret, count, total = 0;

    if (!data || !num) {
        return -1;
    }

    ret = stm32f4_i2c_prepare(i2c);
    if (ret) {
        return ret;
    }

    port = i2c->priv;

    raw_mem_write(&port->regs->DR, (addr << 1) | 1);

    count = 10000;
    while (!(raw_mem_read(&port->regs->SR1) & I2C_SR1_ADDR)) {
        if ((raw_mem_read(&port->regs->SR1) & I2C_SR1_AF) || !count--) {
            /* Clear error */
            raw_mem_clear_bits(&port->regs->SR1, I2C_SR1_AF);
            goto out_err;
        }
    }

    uint8_t single_byte = num == 1;

    if (!single_byte) {
        raw_mem_set_bits(&port->regs->CR1, I2C_CR1_ACK);
    }
    else {  /* In single byte receive, never ACK */
        raw_mem_clear_bits(&port->regs->CR1, I2C_CR1_ACK);
    }

    while (num--) {
        count = 10000;
        while (!(raw_mem_read(&port->regs->SR2) & I2C_SR2_MSL)) {
            if (!count--) {
                goto out_err;
            }
        }

        /* In single byte receive, stop after ADDR clear (SR1 and SR2 read) */
        if (single_byte) {
            raw_mem_set_bits(&port->regs->CR1, I2C_CR1_STOP);
        }

        count = 10000;
        while (!(raw_mem_read(&port->regs->SR1) & I2C_SR1_RXNE)) {
            if (!count--) {
                goto out_err;
            }
        }

        *data++ = raw_mem_read(&port->regs->DR);
        total++;

        /* NACK and STOP after second to last receive */
        if (num == 1) {
            raw_mem_clear_bits(&port->regs->CR1, I2C_CR1_ACK);
            raw_mem_set_bits(&port->regs->CR1, I2C_CR1_STOP);
        }
    }

    release(&i2c->lock);

    return total;

out_err:
    raw_mem_set_bits(&port->regs->CR1, I2C_CR1_STOP);
    release(&i2c->lock);
    return total;
}

static struct i2c_ops stm32f4_i2c_ops = {
    .init = stm32f4_i2c_init,
    .deinit = stm32f4_i2c_deinit,
    .read = stm32f4_i2c_read,
    .write = stm32f4_i2c_write,
};

static int stm32f4_i2c_probe(const char *name) {
    const void *blob = fdtparse_get_blob();
    int offset;

    /* Lookup peripheral node */
    offset = fdt_path_offset(blob, name);
    if (offset < 0) {
        return 0;
    }

    /* Check that peripheral is compatible with driver */
    return fdt_node_check_compatible(blob, offset, STM32F4_I2C_COMPAT) == 0;
}

static struct obj *stm32f4_i2c_ctor(const char *name) {
    const void *blob = fdtparse_get_blob();
    int offset;
    struct obj *obj;
    struct i2c *i2c;
    struct stm32f4_i2c *port;
    struct stm32f4_i2c_regs *regs;
    int err, periph_id;

    offset = fdt_path_offset(blob, name);
    if (offset < 0) {
        return NULL;
    }

    if (fdt_node_check_compatible(blob, offset, STM32F4_I2C_COMPAT)) {
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

    obj = instantiate(name, &i2c_class, &stm32f4_i2c_ops, struct i2c);
    if (!obj) {
        return NULL;
    }

    i2c = to_i2c(obj);

    init_mutex(&i2c->lock);

    i2c->priv = kmalloc(sizeof(struct stm32f4_i2c));
    if (!i2c->priv) {
        goto err_free_obj;
    }

    port = i2c->priv;
    memset(port, 0, sizeof(*port));

    port->ready = 0;
    port->regs = regs;
    port->periph_id = periph_id;

    /* Setup GPIOs */
    for (int i = 0; i < 2; i++) {
        const char *i2c_gpio_props[] = {"i2c,scl-gpio", "i2c,sda-gpio"};
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
        port->gpio[i] = gpio;

        ops = gpio_obj->ops;

        /* High speed GPIOs */
        err = ops->set_flags(gpio, STM32F4_GPIO_SPEED,
                             STM32F4_GPIO_SPEED_50MHZ);
        if (err) {
            goto err_free_gpio;
        }

        /* Open drain GPIOs */
        err = ops->set_flags(gpio, STM32F4_GPIO_TYPE,
                             STM32F4_GPIO_OPEN_DRAIN);
        if (err) {
            goto err_free_gpio;
        }

        /* Pull up */
        err = ops->set_flags(gpio, STM32F4_GPIO_PULL,
                             STM32F4_GPIO_PULL_UP);
        if (err) {
            goto err_free_gpio;
        }

        /* Set to I2C mode */
        err = ops->set_flags(gpio, STM32F4_GPIO_ALT_FUNC,
                             STM32F4_GPIO_AF_I2C);
        if (err) {
            goto err_free_gpio;
        }
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

    kfree(port);

err_free_obj:
    class_deinstantiate(obj);

    return NULL;
}

static struct mutex stm32f4_i2c_driver_mut = INIT_MUTEX;

static struct device_driver stm32f4_i2c_compat_driver = {
    .name = STM32F4_I2C_COMPAT,
    .probe = stm32f4_i2c_probe,
    .ctor = stm32f4_i2c_ctor,
    .class = &i2c_class,
    .mut = &stm32f4_i2c_driver_mut,
};

static int stm32f4_i2c_register(void) {
    /*
     * Used to automatically generate driver entries for all compatible
     * nodes in the device tree.
     */
    device_compat_driver_register(&stm32f4_i2c_compat_driver);
    return 0;
}
CORE_INITIALIZER(stm32f4_i2c_register)
