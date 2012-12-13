#include <stdint.h>
#include <stddef.h>
#include <dev/registers.h>
#include <dev/cortex_m.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dev/resource.h>
#include <kernel/sched.h>
#include <kernel/semaphore.h>
#include <kernel/fault.h>

#include <dev/hw/usart.h>

#define USART_DMA_MSIZE     (512-sizeof(uint32_t))      /* Since malloc headers take up some space, we want to request the max space we can fit in one block */

struct semaphore usart_semaphore;


resource uart_console = {   .writer     = &usart_putc,
                            .swriter    = &usart_puts,
                            .reader     = &usart_getc,
                            .closer     = &usart_close,
                            .env        = NULL,
                            .sem        = &usart_semaphore};

static uint16_t usart_baud(uint32_t baud) __attribute__((section(".kernel")));

char *usart_rx_buf;
char *usart_tx_buf;

uint8_t usart_ready = 0;

void init_usart(void) {
    *RCC_APB2ENR |= RCC_APB2ENR_USART1EN;  /* Enable USART1 Clock */
    *RCC_AHB1ENR |= RCC_AHB1ENR_GPIOBEN;   /* Enable GPIOB Clock */

    /* Set PB6 and PB7 to alternative function USART
     * See stm32f4_ref.pdf pg 141 and stm32f407.pdf pg 51 */

    /* Sets PB6 and PB7 to alternative function mode */
    *GPIOB_MODER &= ~(GPIO_MODER_M(6) | GPIO_MODER_M(7));
    *GPIOB_MODER |= (GPIO_MODER_ALT << GPIO_MODER_PIN(6)) | (GPIO_MODER_ALT << GPIO_MODER_PIN(7));

    /* Sets PB6 and PB7 to USART1-3 mode */
    *GPIOB_AFRL &= ~(GPIO_AFRL_M(6) | GPIO_AFRL_M(7));
    *GPIOB_AFRL |= (GPIO_AF_USART13 << GPIO_AFRL_PIN(6)) | (GPIO_AF_USART13 << GPIO_AFRL_PIN(7));

    /* Sets pin output to push/pull */
    *GPIOB_OTYPER &= ~(GPIO_OTYPER_M(6) | GPIO_OTYPER_M(7));
    *GPIOB_OTYPER |= (GPIO_OTYPER_PP << GPIO_OTYPER_PIN(6)) | (GPIO_OTYPER_PP << GPIO_OTYPER_PIN(7));

    /* No pull-up, no pull-down */
    *GPIOB_PUPDR &= ~(GPIO_PUPDR_M(6) | GPIO_PUPDR_M(7));
    *GPIOB_PUPDR |= (GPIO_PUPDR_NONE << GPIO_PUPDR_PIN(6)) | (GPIO_PUPDR_NONE << GPIO_PUPDR_PIN(7));

    /* Speed to 50Mhz */
    *GPIOB_OSPEEDR &= ~(GPIO_OSPEEDR_M(6) | GPIO_OSPEEDR_M(7));
    *GPIOB_OSPEEDR |= (GPIO_OSPEEDR_50M << GPIO_OSPEEDR_PIN(6)) | (GPIO_OSPEEDR_50M << GPIO_OSPEEDR_PIN(7));

    /* Enable USART1 */
    *USART1_CR1 |= USART_CR1_UE;

    /* 8 data bits */
    *USART1_CR1 &= ~(1 << 12);

    /* 1 stop bit */
    *USART1_CR2 &= ~(3 << 12);

    /** DMA set up **/
    /* DMA2, Stream 2, Channel 4 is USART1_RX
     * DMA2, Stream 7, Channel 4 is USART1_TX */
    *USART1_CR3 |= USART_CR3_DMAR_EN | USART_CR3_DMAT_EN;

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
    *DMA2_PAR_S(2) = (uint32_t) USART1_DR;    /* RX */
    *DMA2_PAR_S(7) = (uint32_t) USART1_DR;    /* TX */

    /* Allocate buffer memory */
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
    *USART1_BRR = usart_baud(115200);

    /* Enable reciever and transmitter */
    *USART1_CR1 |= USART_CR1_RE;
    *USART1_CR1 |= USART_CR1_TE;

    /* Reset semaphore */
    init_semaphore(&usart_semaphore);

    usart_ready = 1;
}

/* Calculates the value for the USART_BRR */
uint16_t usart_baud(uint32_t baud) {
    float usartdiv = (84000000)/(16*(float)baud);
    uint16_t mantissa = (uint16_t) usartdiv;
    float fraction = 16 * (usartdiv-mantissa);
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

void usart_putc(char c, void *env) {
    /* Wait for DMA to be ready */
    while (*DMA2_CR_S(7) & DMA_SxCR_EN) {
        if (task_switching && !IPSR()) {
            SVC(SVC_YIELD);
        }
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
}

void usart_puts(char *s, void *env) {
    while (*s) {
        char *buf = usart_tx_buf;
        uint16_t count = 0;

        /* Wait for DMA to be ready */
        while (*DMA2_CR_S(7) & DMA_SxCR_EN) {
            if (task_switching && !IPSR()) {
                SVC(SVC_YIELD);
            }
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
    }
}

char usart_getc(void *env) {
    static uint32_t read = 0;
    uint16_t dma_read = USART_DMA_MSIZE - (uint16_t) *DMA2_NDTR_S(2);
    uint8_t wrapped = *DMA2_LISR & DMA_LISR_TCIF2;
    char *usart_buf = usart_rx_buf;

    /* Waiting... */
    while (!wrapped && dma_read == read) {
        /* Yield */
        if (task_switching && !IPSR()) {
            /* We release the semaphore here to allow other tasks to print while we wait.
             * This is not an ideal solution, as someone may steal the data we are waiting
             * for, but it prevents tasks like shell, which continuously waits for input,
             * from preventing all other tasks from printing */
            release(&usart_semaphore);
            SVC(SVC_YIELD);
            acquire(&usart_semaphore);
        }

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

void usart_close(resource *resource) {
    panic_print("USART is a fundamental resource, it may not be closed.");
}
