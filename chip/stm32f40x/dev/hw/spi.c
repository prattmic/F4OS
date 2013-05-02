#include <stdint.h>
#include <stddef.h>
#include <dev/cortex_m.h>
#include <dev/registers.h>
#include <dev/hw/gpio.h>
#include <kernel/semaphore.h>

#include <dev/hw/spi.h>

#define SPI_READ    (uint8_t) (1 << 7)

void init_spi1(void) __attribute__((section(".kernel")));
static int spi_send_receive(struct spi_port *spi, uint8_t send, uint8_t *receive) __attribute__((section(".kernel")));

struct spi_port spi1 = {
    .ready = 0,
    .port = 1,
    .init = &init_spi1
};

struct semaphore spi1_semaphore = {
    .lock = 0,
    .held_by = NULL,
    .waiting = NULL
};

void init_spi1(void) {
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
    *SPI_CR1(1) |= SPI_CR1_BR_4 | SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI;

    *SPI_CR1(1) |= SPI_CR1_SPE;

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
    while (!(*SPI_SR(spi->port) & SPI_SR_TXNE)) {
        if (!count--) {
            return -1;
        }
    }
    *SPI_DR(spi->port) = send;

    /* Wait for response
     * Note: this "response" was transmitted while we were
     * transmitting the data above, it is not the device's response to that request. */
    count = 10000;
    while (!(*SPI_SR(spi->port) & SPI_SR_RXNE)) {
        if (!count--) {
            return -1;
        }
    }
    *data = *SPI_DR(spi->port);

    return 0;
}

int spi_write(struct spi_port *spi, struct spi_dev *dev, uint8_t addr, uint8_t *data, uint32_t num) {
    /* Verify valid SPI port */
    if (!spi || !spi->ready || spi->port < 1 || spi->port > 3) {
        return -1;
    }

    /* Verify valid SPI device */
    if (!dev || !dev->cs_high || !dev->cs_low) {
        return -1;
    }

    if (num == 0) {
        return 0;
    }

    if (!data) {
        return -1;
    }

    uint32_t total = 0;

    /* Data MUST be read after each TX */

    /* Clear overrun by reading old data */
    if (*SPI_SR(spi->port) & SPI_SR_OVR) {
        READ_AND_DISCARD(SPI_DR(spi->port));
        READ_AND_DISCARD(SPI_SR(spi->port));
    }

    dev->cs_low();

    /* Transmit address */
    if (spi_send_receive(spi, addr, NULL)) {
        goto fail;
    }

    while (num--) {
        /* Transmit data */
        if (spi_send_receive(spi, *data++, NULL)) {
            goto fail;
        }

        total += 1;
    }

    dev->cs_high();

    return total;

fail:
    dev->cs_high();
    return -1;
}

int spi_read(struct spi_port *spi, struct spi_dev *dev, uint8_t addr, uint8_t *data, uint32_t num) {
    /* Verify valid SPI port */
    if (!spi || !spi->ready || spi->port < 1 || spi->port > 3) {
        return -1;
    }

    /* Verify valid SPI device */
    if (!dev || !dev->cs_high || !dev->cs_low) {
        return -1;
    }

    if (num == 0) {
        return 0;
    }

    if (!data) {
        return -1;
    }

    uint32_t total = 0;

    /* Data MUST be read after each TX */

    /* Clear overrun by reading old data */
    if (*SPI_SR(spi->port) & SPI_SR_OVR) {
        READ_AND_DISCARD(SPI_DR(spi->port));
        READ_AND_DISCARD(SPI_SR(spi->port));
    }

    dev->cs_low();

    /* Transmit address */
    if (spi_send_receive(spi, addr | SPI_READ, NULL)) {
        goto fail;
    }

    while (num--) {
        if (spi_send_receive(spi, 0x00, data++)) {
            goto fail;
        }

        total += 1;
    }

    dev->cs_high();

    return total;

fail:
    dev->cs_high();
    return -1;
}
