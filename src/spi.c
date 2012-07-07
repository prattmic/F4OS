#include "types.h"
#include "registers.h"
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
