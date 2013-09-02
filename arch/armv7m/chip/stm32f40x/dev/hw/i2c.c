#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <arch/chip/gpio.h>
#include <arch/chip/i2c.h>
#include <arch/chip/registers.h>
#include <dev/device.h>
#include <dev/hw/gpio.h>
#include <dev/raw_mem.h>
#include <dev/resource.h>
#include <kernel/fault.h>
#include <kernel/semaphore.h>
#include <kernel/class.h>
#include <kernel/init.h>
#include <mm/mm.h>

#include <dev/hw/i2c.h>

enum {
    I2C_GPIO_SCL,
    I2C_GPIO_SDA,
};

struct stm32f4_i2c {
    uint8_t                 ready;
    struct gpio             *gpio[2];   /* SCL, then SDA */
    struct stm32f4_i2c_regs *regs;
};

/* For now, all I2C ports are configured the same */
static int stm32f4_i2c_init(struct i2c *i2c) {
    struct stm32f4_i2c *port = i2c->priv;

    acquire(&i2c->lock);

    /* Already initialized? */
    if (port->ready) {
        return 0;
    }

    /* Enable I2C clock */
    switch (i2c->num) {
    case 1:
        /* TODO: Clock module like GPIO, to properly handle clocks */
        *RCC_APB1ENR |= RCC_APB1ENR_I2C1EN;
        break;
    case 2:
        *RCC_APB1ENR |= RCC_APB1ENR_I2C2EN;
        break;
    case 3:
        *RCC_APB1ENR |= RCC_APB1ENR_I2C3EN;
        break;
    }

    /* Configure peripheral */
    raw_mem_set_mask(&port->regs->CR2, I2C_CR2_FREQ_MASK, I2C_CR2_FREQ(42));

    /* Set clock to 300kHz */
    raw_mem_set_mask(&port->regs->CCR, I2C_CCR_CCR_MASK, I2C_CCR_CCR(140));
    raw_mem_write(&port->regs->TRISE, 43);

    /* Enable */
    raw_mem_set_bits(&port->regs->CR1, I2C_CR1_PE);

    port->ready = 1;

    release(&i2c->lock);

    return 0;
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
 * Upon successful return, normal I2C use can continue.
 *
 * @param i2c   I2C port to rest
 * @returns 0 on successful reset, negative otherwiese
 */
static int stm32f4_i2c_reset(struct i2c *i2c) {
    struct stm32f4_i2c *port = i2c->priv;
    struct i2c_ops *ops = (struct i2c_ops *) i2c->obj.ops;

    port->ready = 0;

    /* Software reset */
    raw_mem_set_bits(&port->regs->CR1, I2C_CR1_SWRST);

    /* Wait until peripheral is disabled */
    while (raw_mem_read(&port->regs->CR1) & I2C_CR1_PE);

    /* Re-enable */
    raw_mem_clear_bits(&port->regs->CR1, I2C_CR1_SWRST);

    /* Re-initialize */
    return ops->init(i2c);
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

    port = i2c->priv;
    if (!port->ready) {
        struct i2c_ops *ops = (struct i2c_ops *) i2c->obj.ops;
        ret = ops->init(i2c);
        if (ret) {
            return ret;
        }
    }

    acquire(&i2c->lock);

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

/*
 * TODO: The I2C ports can be on multiple sets of pins.
 * Support this.
 */
struct stm32f4_i2c_port {
    char *name;
    int index;
    uint32_t gpio[2];   /* SCL, then SDA */
} stm32f4_i2c_ports[] = {
    { .name = "i2c1", .index = 1, .gpio = {
        [I2C_GPIO_SCL] = STM32F4_GPIO_PB8,
        [I2C_GPIO_SDA] = STM32F4_GPIO_PB9} },
    { .name = "i2c2", .index = 2, .gpio = {
        [I2C_GPIO_SCL] = STM32F4_GPIO_PB10,
        [I2C_GPIO_SDA] = STM32F4_GPIO_PB11} },
    { .name = "i2c3", .index = 3, .gpio = {
        [I2C_GPIO_SCL] = STM32F4_GPIO_PA8,
        [I2C_GPIO_SCL] = STM32F4_GPIO_PC9} },
};

#define NUM_I2C_PORTS   (sizeof(stm32f4_i2c_ports)/sizeof(stm32f4_i2c_ports[0]))

static struct stm32f4_i2c_port *stm32f4_i2c_get_config(const char *name) {
    for (int i = 0; i < NUM_I2C_PORTS; i++) {
        if (strncmp((char *)name, stm32f4_i2c_ports[i].name, 5) == 0) {
            return &stm32f4_i2c_ports[i];
        }
    }

    return NULL;
}

static int stm32f4_i2c_probe(const char *name) {
    return !!stm32f4_i2c_get_config(name);
}

static struct obj *stm32f4_i2c_ctor(const char *name) {
    struct stm32f4_i2c_port *config = stm32f4_i2c_get_config(name);
    struct obj *obj;
    struct i2c *i2c;
    struct stm32f4_i2c *port;

    if (!config) {
        return NULL;
    }

    obj = instantiate(config->name, &i2c_class, &stm32f4_i2c_ops, struct i2c);
    if (!obj) {
        return NULL;
    }

    i2c = to_i2c(obj);

    init_semaphore(&i2c->lock);

    i2c->num = config->index;

    i2c->priv = kmalloc(sizeof(struct stm32f4_i2c));
    if (!i2c->priv) {
        goto err_free_obj;
    }

    port = i2c->priv;
    memset(port, 0, sizeof(*port));

    port->ready = 0;
    port->regs = i2c_get_regs(i2c->num);

    /* Setup GPIOs */
    for (int i = 0; i < 2; i++) {
        struct obj *gpio_obj;
        struct gpio *gpio;
        struct gpio_ops *ops;
        int err;

        gpio_obj = gpio_get(config->gpio[i]);
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
    kfree(obj);

    return NULL;
}

static int stm32f4_i2c_register(void) {
    for (int i = 0; i < NUM_I2C_PORTS; i++) {
        struct device_driver *drv = kmalloc(sizeof(*drv));
        if (!drv) {
            return -1;
        }

        struct semaphore *sem = kmalloc(sizeof(*sem));
        if (!sem) {
            kfree(drv);
            return -1;
        }

        init_semaphore(sem);

        drv->name = stm32f4_i2c_ports[i].name;
        drv->probe = stm32f4_i2c_probe;
        drv->ctor = stm32f4_i2c_ctor;
        drv->class = &i2c_class;
        drv->sem = sem;

        device_driver_register(drv);
    }

    return 0;
}
CORE_INITIALIZER(stm32f4_i2c_register)
