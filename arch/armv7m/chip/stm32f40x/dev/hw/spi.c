#include <stddef.h>
#include <stdint.h>
#include <arch/system.h>
#include <arch/chip/gpio.h>
#include <arch/chip/registers.h>
#include <kernel/semaphore.h>

#include <dev/hw/spi.h>

void init_spi1(void) __attribute__((section(".kernel")));
static int spi_send_receive(struct spi_port *spi, uint8_t send, uint8_t *receive) __attribute__((section(".kernel")));

struct spi_port spi1 = {
    .ready = 0,
    .regs = NULL,
    .init = &init_spi1
};

struct semaphore spi1_semaphore = INIT_SEMAPHORE;

void init_spi1(void) {
    spi1.regs = get_spi(1);

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
    spi1.regs->CR1 |= SPI_CR1_BR_4 | SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI;

    spi1.regs->CR1 |= SPI_CR1_SPE;

    init_semaphore(&spi1_semaphore);

    spi1.ready = 1;
}

static int spi_send_receive(struct spi_port *spi, uint8_t send, uint8_t *receive) {
    uint8_t *data;
    uint8_t null;
    int count;

    if (!spi) {
        return -1;
    }

    /* Provide a black hole to write to if receive is NULL */
    if (receive) {
        data = receive;
    }
    else {
        data = &null;
    }

    /* Transmit data */
    count = 10000;
    while (!(spi->regs->SR & SPI_SR_TXNE)) {
        if (!count--) {
            return -1;
        }
    }
    spi->regs->DR = send;

    /* Wait for response
     * Note: this "response" was transmitted while we were
     * transmitting the data above, it is not the device's response to that request. */
    count = 10000;
    while (!(spi->regs->SR & SPI_SR_RXNE)) {
        if (!count--) {
            return -1;
        }
    }
    *data = spi->regs->DR;

    return 0;
}

int spi_read_write(struct spi_port *spi, struct spi_dev *dev, uint8_t *read_data, uint8_t *write_data, uint32_t num) {
    /* Verify valid SPI */
    if (!spi || !spi->ready) {
        return -1;
    }

    /* Verify valid SPI device */
    if (!dev || !dev->cs_high || !dev->cs_low) {
        return -1;
    }

    if (num == 0) {
        return 0;
    }

    uint32_t total = 0;
    int ret;

    /* Data MUST be read after each TX */

    /* Clear overrun by reading old data */
    if (spi->regs->SR & SPI_SR_OVR) {
        READ_AND_DISCARD(&spi->regs->DR);
        READ_AND_DISCARD(&spi->regs->SR);
    }

    if (!dev->extended_transaction) {
        dev->cs_low();
    }

    while (num--) {
        /* Handle NULL read_data and write_data */
        uint8_t send = write_data ? *write_data++ : 0;
        uint8_t *receive = read_data ? read_data++ : NULL;

        /* Transmit data */
        if (spi_send_receive(spi, send, receive)) {
            ret = -1;
            goto out;
        }

        total += 1;
    }

    ret = total;

out:
    if (!dev->extended_transaction) {
        dev->cs_high();
    }

    return ret;
}

void spi_start_transaction(struct spi_port *spi, struct spi_dev *dev) {
    dev->cs_low();
    dev->extended_transaction = 1;
}

void spi_end_transaction(struct spi_port *spi, struct spi_dev *dev) {
    dev->cs_high();
    dev->extended_transaction = 0;
}
