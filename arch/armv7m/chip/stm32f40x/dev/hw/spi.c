#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <arch/system.h>
#include <arch/chip/gpio.h>
#include <arch/chip/spi.h>
#include <arch/chip/registers.h>
#include <dev/device.h>
#include <dev/hw/gpio.h>
#include <kernel/semaphore.h>
#include <kernel/class.h>
#include <kernel/init.h>
#include <mm/mm.h>

#include <dev/hw/spi.h>

struct stm32f4_spi {
    uint8_t                 ready;
    struct gpio             *gpio[3];    /* Each SPI port uses 3 GPIOs */
    struct stm32f4_spi_regs *regs;
};

static int init_spi1(struct spi *s) {
    struct stm32f4_spi *port = (struct stm32f4_spi *) s->priv;

    port->regs = get_spi(1);

    *RCC_APB2ENR |= RCC_APB2ENR_SPI1EN;     /* Enable SPI1 Clock */

    /* Baud = fPCLK/8, Clock high on idle, Capture on rising edge, 16-bit data format */
    port->regs->CR1 |= SPI_CR1_BR_4 | SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI;

    port->regs->CR1 |= SPI_CR1_SPE;

    return 0;
}

static int stm32f4_spi_init(struct spi *s) {
    int ret;
    struct stm32f4_spi *port = (struct stm32f4_spi *) s->priv;

    acquire(&s->lock);

    /* Already initialized? */
    if (port->ready) {
        return 0;
    }

    switch (s->num) {
    case 1:
        ret = init_spi1(s);
        break;
    default:
        ret = -1;
    }

    if (ret == 0) {
        port->ready = 1;
    }

    release(&s->lock);

    return ret;
}

static int stm32f4_spi_deinit(struct spi *s) {
    /* Turn off clocks? */
    /* Release GPIOs? */

    return 0;
}

static int stm32f4_spi_send_receive(struct spi *spi, uint8_t send,
                                    uint8_t *receive) {
    uint8_t *data;
    uint8_t null;
    int count;
    struct stm32f4_spi *port = (struct stm32f4_spi *) spi->priv;

    /* Provide a black hole to write to if receive is NULL */
    if (receive) {
        data = receive;
    }
    else {
        data = &null;
    }

    /* Transmit data */
    count = 10000;
    while (!(port->regs->SR & SPI_SR_TXNE)) {
        if (!count--) {
            return -1;
        }
    }
    port->regs->DR = send;

    /* Wait for response
     * Note: this "response" was transmitted while we were
     * transmitting the data above, it is not the device's response to that request. */
    count = 10000;
    while (!(port->regs->SR & SPI_SR_RXNE)) {
        if (!count--) {
            return -1;
        }
    }
    *data = port->regs->DR;

    return 0;
}

static int stm32f4_spi_read_write(struct spi *spi, struct spi_dev *dev,
                                  uint8_t *read_data, uint8_t *write_data,
                                  uint32_t num) {
    /* Verify valid SPI */
    if (!spi) {
        return -1;
    }

    struct stm32f4_spi *port = (struct stm32f4_spi *) spi->priv;

    /* Initialized? */
    if (!port || !port->ready) {
        struct spi_ops *ops = (struct spi_ops *) spi->obj.ops;
        ops->init(spi);
    }

    /* Verify valid SPI device */
    if (!dev || !dev->cs) {
        return -1;
    }

    if (num == 0) {
        return 0;
    }

    struct gpio_ops *cs_ops = (struct gpio_ops *) dev->cs->obj.ops;
    uint32_t total = 0;
    int ret;

    if (!dev->extended_transaction) {
        acquire(&spi->lock);
        cs_ops->set_output_value(dev->cs, 0);
    }

    /* Data MUST be read after each TX */

    /* Clear overrun by reading old data */
    if (port->regs->SR & SPI_SR_OVR) {
        READ_AND_DISCARD(&port->regs->DR);
        READ_AND_DISCARD(&port->regs->SR);
    }

    while (num--) {
        /* Handle NULL read_data and write_data */
        uint8_t send = write_data ? *write_data++ : 0;
        uint8_t *receive = read_data ? read_data++ : NULL;

        /* Transmit data */
        if (stm32f4_spi_send_receive(spi, send, receive)) {
            ret = -1;
            goto out;
        }

        total += 1;
    }

    ret = total;

out:
    if (!dev->extended_transaction) {
        cs_ops->set_output_value(dev->cs, 1);
        release(&spi->lock);
    }

    return ret;
}

static int stm32f4_spi_write(struct spi *spi, struct spi_dev *dev,
                             uint8_t *data, uint32_t num) {
    return stm32f4_spi_read_write(spi, dev, NULL, data, num);
}

static int stm32f4_spi_read(struct spi *spi, struct spi_dev *dev,
                            uint8_t *data, uint32_t num) {
    return stm32f4_spi_read_write(spi, dev, data, NULL, num);
}

static void stm32f4_spi_start_transaction(struct spi *spi, struct spi_dev *dev) {
    struct gpio_ops *cs_ops = (struct gpio_ops *) dev->cs->obj.ops;

    acquire(&spi->lock);
    cs_ops->set_output_value(dev->cs, 0);
    dev->extended_transaction = 1;
}

static void stm32f4_spi_end_transaction(struct spi *spi, struct spi_dev *dev) {
    struct gpio_ops *cs_ops = (struct gpio_ops *) dev->cs->obj.ops;

    dev->extended_transaction = 0;
    cs_ops->set_output_value(dev->cs, 1);
    release(&spi->lock);
}

struct spi_ops stm32f4_spi_ops = {
    .init = stm32f4_spi_init,
    .deinit = stm32f4_spi_deinit,
    .read_write = stm32f4_spi_read_write,
    .read = stm32f4_spi_read,
    .write = stm32f4_spi_write,
    .start_transaction = stm32f4_spi_start_transaction,
    .end_transaction = stm32f4_spi_end_transaction,
};

struct stm32f4_spi_port {
    char *name;
    int index;
    int function;       /* GPIO AF */
    uint32_t gpio[3];   /* SCK, MOSI, MISO, order invariant */
} stm32f4_spi_ports[] = {
    { .name = "spi1", .index = 1, .function = STM32F4_GPIO_AF_SPI1,
        .gpio = {STM32F4_GPIO_PA5, STM32F4_GPIO_PA6, STM32F4_GPIO_PA7} },
};

#define NUM_SPI_PORTS   (sizeof(stm32f4_spi_ports)/sizeof(stm32f4_spi_ports[0]))

static struct stm32f4_spi_port *stm32f4_get_config(const char *name) {
    for (int i = 0; i < NUM_SPI_PORTS; i++) {
        if (strncmp((char *)name, stm32f4_spi_ports[i].name, 5) == 0) {
            return &stm32f4_spi_ports[i];
        }
    }

    return NULL;
}

static int stm32f4_spi_probe(const char *name) {
    return !!stm32f4_get_config(name);
}

static struct obj *stm32f4_spi_ctor(const char *name) {
    struct stm32f4_spi_port *config = stm32f4_get_config(name);
    struct obj *obj;
    struct spi *spi;
    struct stm32f4_spi *port;

    if (!config) {
        return NULL;
    }

    obj = instantiate(config->name, &spi_class, &stm32f4_spi_ops, struct spi);
    if (!obj) {
        return NULL;
    }

    spi = to_spi(obj);

    init_semaphore(&spi->lock);

    /* May be unnecessary */
    spi->num = config->index;

    spi->priv = kmalloc(sizeof(struct stm32f4_spi));
    if (!spi->priv) {
        goto err_free_obj;
    }

    port = spi->priv;
    memset(port, 0, sizeof(*port));

    port->ready = 0;

    /* Setup GPIOs */
    for (int i = 0; i < 3; i++) {
        struct obj *gpio_obj;
        struct gpio *gpio;
        struct gpio_ops *ops;

        gpio_obj = gpio_get(config->gpio[i]);
        if (!gpio_obj) {
            goto err_free_gpio;
        }

        gpio = to_gpio(gpio_obj);

        ops = gpio_obj->ops;

        ops->set_flags(gpio, STM32F4_GPIO_SPEED,
                       STM32F4_GPIO_SPEED_50MHZ);
        ops->set_flags(gpio, STM32F4_GPIO_ALT_FUNC,
                       config->function);

        port->gpio[i] = gpio;
    }

    /* Export to the OS */
    class_export_member(obj);

    return obj;

err_free_gpio:
    for (int i = 0; i < 3; i++) {
        if (port->gpio[i]) {
            gpio_put(&port->gpio[i]->obj);
        }
    }

    kfree(port);

err_free_obj:
    kfree(obj);

    return NULL;
}

static int stm32f4_spi_register(void) {
    for (int i = 0; i < NUM_SPI_PORTS; i++) {
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

        drv->name = stm32f4_spi_ports[i].name;
        drv->probe = stm32f4_spi_probe;
        drv->ctor = stm32f4_spi_ctor;
        drv->class = &spi_class;
        drv->sem = sem;

        device_driver_register(drv);
    }

    return 0;
}
CORE_INITIALIZER(stm32f4_spi_register)
