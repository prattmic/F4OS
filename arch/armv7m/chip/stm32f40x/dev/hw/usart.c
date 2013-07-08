#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arch/system.h>
#include <arch/chip/clock.h>
#include <arch/chip/gpio.h>
#include <arch/chip/registers.h>
#include <dev/resource.h>
#include <kernel/fault.h>
#include <kernel/sched.h>
#include <kernel/semaphore.h>
#include <mm/mm.h>

#include <dev/hw/usart.h>

/* Since malloc headers take up some space,
 * we want to request a little less space,
 * so we don't end up wasting a bunch of
 * space when the header causes us to get
 * 1KB allocated. */
#define USART_DMA_MSIZE     (500)

struct semaphore usart_read_semaphore = INIT_SEMAPHORE;
struct semaphore usart_write_semaphore = INIT_SEMAPHORE;

resource usart_resource = { .writer     = &usart_putc,
                            .swriter    = &usart_puts,
                            .reader     = &usart_getc,
                            .closer     = &usart_close,
                            .env        = NULL,
                            .read_sem   = &usart_read_semaphore,
                            .write_sem  = &usart_write_semaphore};

static uint16_t usart_baud(uint32_t baud) __attribute__((section(".kernel")));

char *usart_rx_buf;
char *usart_tx_buf;

uint8_t usart_ready = 0;

void init_usart(void) {
    usart_t *usart1 = get_usart(1);

    *RCC_APB2ENR |= RCC_APB2ENR_USART1EN;  /* Enable USART1 Clock */
    *RCC_AHB1ENR |= RCC_AHB1ENR_GPIOBEN;   /* Enable GPIOB Clock */

    /* Set PB6 and PB7 to alternative function USART
     * See stm32f4_ref.pdf pg 141 and stm32f407.pdf pg 51 */

    /* PB6 */
    gpio_moder(GPIOB, 6, GPIO_MODER_ALT);
    gpio_afr(GPIOB, 6, GPIO_AF_USART13);
    gpio_otyper(GPIOB, 6, GPIO_OTYPER_PP);
    gpio_pupdr(GPIOB, 6, GPIO_PUPDR_NONE);
    gpio_ospeedr(GPIOB, 6, GPIO_OSPEEDR_50M);

    /* PB7 */
    gpio_moder(GPIOB, 7, GPIO_MODER_ALT);
    gpio_afr(GPIOB, 7, GPIO_AF_USART13);
    gpio_otyper(GPIOB, 7, GPIO_OTYPER_PP);
    gpio_pupdr(GPIOB, 7, GPIO_PUPDR_NONE);
    gpio_ospeedr(GPIOB, 7, GPIO_OSPEEDR_50M);

    /* Enable USART1 */
    usart1->CR1 |= USART_CR1_UE;

    /* 8 data bits */
    usart1->CR1 &= ~(1 << 12);

    /* 1 stop bit */
    usart1->CR2 &= ~(3 << 12);

    /** DMA set up **/
    /* DMA2, Stream 2, Channel 4 is USART1_RX
     * DMA2, Stream 7, Channel 4 is USART1_TX */
    usart1->CR3 |= USART_CR3_DMAR | USART_CR3_DMAT;

    /* Enable DMA2 clock */
    *RCC_AHB1ENR |= RCC_AHB1ENR_DMA2EN;
    *RCC_AHB1ENR |= RCC_AHB1ENR_DMA1EN;

    /* Clear configuration registers enable bits and wait for them to be ready */
    *DMA2_CR_S(2) &= ~(DMA_SxCR_EN);
    *DMA2_CR_S(7) &= ~(DMA_SxCR_EN);
    while ( (*DMA2_CR_S(2) & DMA_SxCR_EN) || (*DMA2_CR_S(7) & DMA_SxCR_EN) );

    /* Select channel 4 */
    *DMA2_CR_S(2) |= DMA_SxCR_CHSEL(4);
    *DMA2_CR_S(7) |= DMA_SxCR_CHSEL(4);

    /* Peripheral address - Both use USART data register */
    *DMA2_PAR_S(2) = (uint32_t) &usart1->DR;    /* RX */
    *DMA2_PAR_S(7) = (uint32_t) &usart1->DR;    /* TX */

    /*
     * Allocate buffer memory.
     * This must be allocated because static data goes into
     * CCMRAM on STM32F4 implementation, and the bus matrix
     * does not connect CCMRAM to the DMA engine. Thus, it
     * is allocated from user heap to ensure it is accessible
     * by DMA
     */
    usart_rx_buf = (char *) malloc(USART_DMA_MSIZE);
    usart_tx_buf = (char *) malloc(USART_DMA_MSIZE);
    if ((usart_rx_buf == NULL) || (usart_tx_buf == NULL)) {
        panic();
    }
    else {
        /* Clear buffers */
        memset(usart_rx_buf, 0, USART_DMA_MSIZE);
        memset(usart_tx_buf, 0, USART_DMA_MSIZE);
        *DMA2_M0AR_S(2) = (uint32_t) usart_rx_buf;
        *DMA2_M0AR_S(7) = (uint32_t) usart_tx_buf;
    }

    /* Number of data items to be transferred */
    *DMA2_NDTR_S(2) = (uint16_t) USART_DMA_MSIZE;

    /* FIFO setup */
    *DMA2_FCR_S(7) |= DMA_SxFCR_FTH_4 | DMA_SxFCR_DMDIS;

    /* Data direct, memory increment, high priority, memory burst */
    *DMA2_CR_S(2) |= DMA_SxCR_DIR_PM | DMA_SxCR_MINC | DMA_SxCR_PL_HIGH | DMA_SxCR_CIRC;
    *DMA2_CR_S(7) |= DMA_SxCR_DIR_MP | DMA_SxCR_MINC | DMA_SxCR_PL_HIGH | DMA_SxCR_MBURST_4;

    /* Enable DMAs */
    *DMA2_CR_S(2) |= DMA_SxCR_EN;

    /** DMA End **/

    /* Set baud rate */
    usart1->BRR = usart_baud(115200);

    /* Enable reciever and transmitter */
    usart1->CR1 |= USART_CR1_RE;
    usart1->CR1 |= USART_CR1_TE;

    usart_ready = 1;
}

/* Calculates the value for the USART_BRR */
uint16_t usart_baud(uint32_t baud) {
    float usartdiv = APB2_CLOCK/(16.0*baud);
    uint16_t mantissa = (uint16_t) usartdiv;
    float fraction = 16 * (usartdiv - mantissa);
    uint16_t int_fraction = (uint16_t) fraction;

    /* Round fraction */
    while (fraction > 1) {
        fraction--;
    }
    if (fraction >= 0.5f) {
        int_fraction += 1;
    }

    if (int_fraction == 16) {
        mantissa += 1;
        int_fraction = 0;
    }

    return (mantissa << 4) | int_fraction;
}

int usart_putc(char c, void *env) {
    if (!usart_ready) {
        return -1;
    }

    /* Wait for DMA to be ready */
    while (*DMA2_CR_S(7) & DMA_SxCR_EN) {
        yield_if_possible();
    }

    if (*DMA2_HISR & DMA_HISR_TCIF7) {
        /* Clear transfer complete flag */
        *DMA2_HIFCR |= DMA_HIFCR_CTCIF7;
    }

    *usart_tx_buf = c;

    /* 1 byte to write */
    *DMA2_NDTR_S(7) = 1;
    /* Enable DMA */
    *DMA2_CR_S(7) |= DMA_SxCR_EN;

    return 1;
}

int usart_puts(char *s, void *env) {
    if (!usart_ready) {
        return -1;
    }

    int total = 0;

    while (*s) {
        char *buf = usart_tx_buf;
        uint16_t count = 0;

        /* Wait for DMA to be ready */
        while (*DMA2_CR_S(7) & DMA_SxCR_EN) {
            yield_if_possible();
        }

        if (*DMA2_HISR & DMA_HISR_TCIF7) {
            /* Clear transfer complete flag */
            *DMA2_HIFCR |= DMA_HIFCR_CTCIF7;
        }

        /* Copy into buffer */
        while (*s && count < USART_DMA_MSIZE) {
            count += 1;
            *buf++ = *s++;
        }

        /* Number of bytes to write */
        *DMA2_NDTR_S(7) = (uint16_t) count;
        /* Enable DMA */
        *DMA2_CR_S(7) |= DMA_SxCR_EN;

        total += count;
    }

    return total;
}

char usart_getc(void *env, int *error) {
    if (!usart_ready) {
        if (error != NULL) {
            *error = -1;
        }
        return 0;
    }

    if (error != NULL) {
        *error = 0;
    }

    static uint32_t read = 0;
    uint16_t dma_read = USART_DMA_MSIZE - (uint16_t) *DMA2_NDTR_S(2);
    uint8_t wrapped = *DMA2_LISR & DMA_LISR_TCIF2;
    char *usart_buf = usart_rx_buf;

    /* Waiting... */
    while (!wrapped && dma_read == read) {
        yield_if_possible();

        dma_read = USART_DMA_MSIZE - (uint16_t) *DMA2_NDTR_S(2);
        wrapped = *DMA2_LISR & DMA_LISR_TCIF2;
    }

    /* DMA has not wrapped around and is ahead of us */
    if (!wrapped && dma_read >= read) {
        read += 1;
        return *(usart_buf + (read-1));
    }
    /* The DMA has not wrapped around, yet is somehow behind us, start over */
    else if (!wrapped && dma_read < read) {
        read = 1;
        return *usart_buf;
    }
    /* The DMA has wrapped around, but hasn't caught up to us yet */
    else if (wrapped && dma_read < read) {
        read += 1;
        return *(usart_buf + (read-1));
    }
    /* The DMA has wrapped around, and has already caught up to us, start over */
    else {
        /* Clear completion flag */
        *DMA2_LIFCR |= DMA_LIFCR_CTCIF2;
        read = 1;
        return *usart_buf;
    }
}

int usart_close(resource *resource) {
    printk("OOPS: USART is a fundamental resource, it may not be closed.");
    return -1;
}
