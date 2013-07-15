#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <arch/system.h>
#include <arch/chip/gpio.h>
#include <arch/chip/spi.h>
#include <arch/chip/registers.h>
#include <dev/hw/gpio.h>
#include <kernel/semaphore.h>
#include <kernel/class.h>
#include <kernel/init.h>
#include <mm/mm.h>

#include <dev/hw/spi.h>

struct stm32f4_spi {
    uint8_t                 ready;
    struct stm32f4_spi_regs *regs;
};

static int init_spi1(struct spi *s) {
    struct stm32f4_spi *port = (struct stm32f4_spi *) s->priv;

    port->regs = get_spi(1);

    *RCC_APB2ENR |= RCC_APB2ENR_SPI1EN;     /* Enable SPI1 Clock */
    *RCC_AHB1ENR |= RCC_AHB1ENR_GPIOAEN;    /* Enable GPIOA Clock */

    /* Set PA5, PA6, and PA7 to alternative function SPI1
     * See stm32f4_ref.pdf pg 141 and stm32f407.pdf pg 51 */

    /* PA5 */
    gpio_moder(GPIOA, 5, GPIO_MODER_ALT);
    gpio_afr(GPIOA, 5, GPIO_AF_SPI12);
    gpio_otyper(GPIOA, 5, GPIO_OTYPER_PP);
    gpio_pupdr(GPIOA, 5, GPIO_PUPDR_NONE);
    gpio_ospeedr(GPIOA, 5, GPIO_OSPEEDR_50M);

    /* PA6 */
    gpio_moder(GPIOA, 6, GPIO_MODER_ALT);
    gpio_afr(GPIOA, 6, GPIO_AF_SPI12);
    gpio_otyper(GPIOA, 6, GPIO_OTYPER_PP);
    gpio_pupdr(GPIOA, 6, GPIO_PUPDR_NONE);
    gpio_ospeedr(GPIOA, 6, GPIO_OSPEEDR_50M);

    /* PA7 */
    gpio_moder(GPIOA, 7, GPIO_MODER_ALT);
    gpio_afr(GPIOA, 7, GPIO_AF_SPI12);
    gpio_otyper(GPIOA, 7, GPIO_OTYPER_PP);
    gpio_pupdr(GPIOA, 7, GPIO_PUPDR_NONE);
    gpio_ospeedr(GPIOA, 7, GPIO_OSPEEDR_50M);

    /* Baud = fPCLK/8, Clock high on idle, Capture on rising edge, 16-bit data format */
    port->regs->CR1 |= SPI_CR1_BR_4 | SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI;

    port->regs->CR1 |= SPI_CR1_SPE;

    return 0;
}

static int stm32f4_spi_init(struct spi *s) {
    int ret;
    struct stm32f4_spi *port = (struct stm32f4_spi *) s->priv;

    /* Already initialized? */
    if (port && port->ready) {
        return 0;
    }
    else if (!port) {
        s->priv = kmalloc(sizeof(struct stm32f4_spi));
        port = s->priv;
        if (!port) {
            return -1;
        }
    }

    port->ready = 0;

    init_semaphore(&s->lock);

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
    else {
        kfree(port);
    }

    return ret;
}

static int stm32f4_spi_deinit(struct spi *s) {
    /* Turn off clocks? */

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

struct stm32f4_spi_ports {
    char *name;
    int index;
} stm32f4_spi_ports[] = {
    { .name = "spi1", .index = 1 },
};

#define NUM_SPI_PORTS   (sizeof(stm32f4_spi_ports)/sizeof(stm32f4_spi_ports[0]))

/* Probe may be the wrong word */
static int stm32f4_spi_probe(void) {
    for (int i = 0; i < NUM_SPI_PORTS; i++) {
        struct obj *o = instantiate(stm32f4_spi_ports[i].name, &spi_class,
                                    &stm32f4_spi_ops, struct spi);
        if (!o) {
            return -1;
        }

        struct spi *s = (struct spi *) to_spi(o);

        s->num = stm32f4_spi_ports[i].index;
        s->priv = NULL;

        /* Export to the OS */
        class_export_member(o);
    }

    return 0;
}
CORE_INITIALIZER(stm32f4_spi_probe)
