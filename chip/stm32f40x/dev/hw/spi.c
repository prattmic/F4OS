#include <stdint.h>
#include <stddef.h>
#include <dev/cortex_m.h>
#include <dev/registers.h>
#include <dev/hw/gpio.h>
#include <kernel/semaphore.h>

#include <dev/hw/spi.h>

#define SPI_READ    (uint8_t) (1 << 7)

void init_spi1(void) __attribute__((section(".kernel")));

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
    while (!(*SPI_SR(spi->port) & SPI_SR_TXNE));
    *SPI_DR(spi->port) = addr;

    /* Wait for response, discard */
    while (!(*SPI_SR(spi->port) & SPI_SR_RXNE));
    READ_AND_DISCARD(SPI_DR(spi->port));

    while (num--) {
        /* Transmit data */
        while (!(*SPI_SR(spi->port) & SPI_SR_TXNE));
        *SPI_DR(spi->port) = *data++;

        /* Wait for response, discard */
        while (!(*SPI_SR(spi->port) & SPI_SR_RXNE));
        READ_AND_DISCARD(SPI_DR(spi->port));

        total += 1;
    }

    dev->cs_high();

    return total;
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
    while (!(*SPI_SR(spi->port) & SPI_SR_TXNE));
    *SPI_DR(spi->port) = (addr | SPI_READ);

    /* Wait for response, discard 
     * Note: this "response" was transmitted while we were
     * transmitting the address, it is not the data we want. */
    while (!(*SPI_SR(spi->port) & SPI_SR_RXNE));
    READ_AND_DISCARD(SPI_DR(spi->port));

    while (num--) {
        /* Transmit zeros while reading response */
        while (!(*SPI_SR(spi->port) & SPI_SR_TXNE));
        *SPI_DR(spi->port) = 0x00;

        /* Wait for response, save it */
        while (!(*SPI_SR(spi->port) & SPI_SR_RXNE));
        *data++ = *SPI_DR(spi->port);

        total += 1;
    }

    dev->cs_high();

    return total;
}
