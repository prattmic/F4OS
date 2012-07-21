#include "types.h"
#include "registers.h"
#include "task.h"
#include "semaphore.h"
#include "usart.h"
#include "spi.h"

void init_spi(void) {
    *RCC_APB2ENR |= RCC_APB2ENR_SPI1EN;     /* Enable SPI1 Clock */
    *RCC_AHB1ENR |= RCC_AHB1ENR_GPIOAEN;    /* Enable GPIOA Clock */

    /* Set PA5, PA6, and PA7 to alternative function SPI1
     * See stm32f4_ref.pdf pg 141 and stm32f407.pdf pg 51 */

    /* Sets PA5, PA6, and PA7 to alternative function mode */
    *GPIOA_MODER &= ~((3 << (5 * 2)) | (3 << (6 * 2)) | (3 << (7 * 2)));
    *GPIOA_MODER |= (GPIO_MODER_ALT << (5 * 2)) | (GPIO_MODER_ALT << (6 * 2)) | (GPIO_MODER_ALT << (7 * 2));

    /* Sets PA5, PA6, and PA7 to SPI1-2 mode */
    *GPIOA_AFRL  &= ~((0xF << (5 * 4)) | (0xF << (6 * 4)) | (0xF << (7 * 4)));
    *GPIOA_AFRL  |= (GPIO_AF_SPI12 << (5 * 4)) | (GPIO_AF_SPI12 << (6 * 4)) | (GPIO_AF_SPI12 << (7 * 4));

    /* Sets pin output to push/pull */
    *GPIOA_OTYPER &= ~((1 << 5) | (1 << 6) | (1 << 7));

    /* No pull-up, no pull-down */
    *GPIOA_PUPDR  &= ~((3 << (5 * 2)) | (3 << (6 * 2)) | (3 << (7 * 2)));

    /* Speed to 50Mhz */
    *GPIOA_OSPEEDR &= ~((3 << (5 * 2)) | (3 << (6 * 2)) | (3 << (7 * 2)));
    *GPIOA_OSPEEDR |= (2 << (5 * 2)) | (2 << (6 * 2)) | (2 << (7 * 2));

    /* Baud = fPCLK/8, Clock high on idle, Capture on rising edge, 16-bit data format */
    //*SPI1_CR1 |= SPI_CR1_BR_256 | SPI_CR1_CPOL | SPI_CR1_CPHA | SPI_CR1_DFF | SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_LSBFIRST;
    *SPI1_CR1 |= SPI_CR1_BR_4 | SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI;

    *SPI1_CR1 |= SPI_CR1_SPE;


    /* GPIO PE3 is chip select */
    *RCC_AHB1ENR |= RCC_AHB1ENR_GPIOEEN;    /* Enable GPIOE Clock */

    /* Sets PE3 to output mode */
    *GPIOE_MODER &= ~(3 << (3 * 2));
    *GPIOE_MODER |= (GPIO_MODER_OUT << (3 * 2));

    /* Sets pin output to push/pull */
    *GPIOE_OTYPER &= ~(1 << 3);

    /* No pull-up, no pull-down */
    *GPIOE_PUPDR  &= ~(3 << (3 * 2));

    /* Speed to 50Mhz */
    *GPIOE_OSPEEDR &= ~(3 << (3 * 2));
    *GPIOE_OSPEEDR |= (2 << (3 * 2));

    /* Set high */
    spi_cs_high();
}

uint8_t spi_write(uint8_t addr, uint8_t data) {
    /* Data MUST be read after each TX */
    volatile uint8_t read;

    /* Clear overrun by reading old data */
    if (*SPI1_SR & SPI_SR_OVR) {
        read = *SPI1_DR;
        read = *SPI1_SR;
    }

    spi_cs_low();

    while (!(*SPI1_SR & SPI_SR_TXNE));
    *SPI1_DR = addr;

    while (!(*SPI1_SR & SPI_SR_RXNE));

    read = *SPI1_DR;

    while (!(*SPI1_SR & SPI_SR_TXNE));
    *SPI1_DR = data;

    while (!(*SPI1_SR & SPI_SR_RXNE));

    spi_cs_high();

    read = *SPI1_DR;

    return read;
}

uint8_t spi_read(uint8_t addr) {
    /* Data MUST be read after each TX */
    volatile uint8_t read;

    /* Clear overrun by reading old data */
    if (*SPI1_SR & SPI_SR_OVR) {
        read = *SPI1_DR;
        read = *SPI1_SR;
    }

    spi_cs_low();

    while (!(*SPI1_SR & SPI_SR_TXNE));

    *SPI1_DR = (addr | SPI_READ);

    while (!(*SPI1_SR & SPI_SR_RXNE));
    read = *SPI1_DR;

    while (!(*SPI1_SR & SPI_SR_TXNE));
    *SPI1_DR = 0x00;

    while (!(*SPI1_SR & SPI_SR_RXNE));

    spi_cs_high();

    read = *SPI1_DR;

    return read;
}

void accel_setup() {
    /* Run this setup, then spi_read() addresses, 0x29 and 0x2A are the X axis */
    spi_write(0x20, 0x47);
}

void accel_loop() {
    accel_setup();

    while (1) {
        uint32_t x = (spi_read(0x28) << 8) | spi_read(0x29);
        uint32_t y = (spi_read(0x2A) << 8) | spi_read(0x2B);
        uint32_t z = (spi_read(0x2C) << 8) | spi_read(0x2D);

        printf("X: %i   Y: %i   Z: %i\r\n", x, y, z);
    }
}
