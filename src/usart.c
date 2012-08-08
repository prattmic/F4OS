#include "types.h"
#include "registers.h"
#include "interrupt.h"
#include "task.h"
#include "context.h"
#include "semaphore.h"
#include "mem.h"
#include "buddy.h"
#include "stdarg.h"
#include "stdio.h"
#include "usart.h"

void init_usart(void) {
    *RCC_APB2ENR |= (1 << 4);   /* Enable USART1 Clock */
    *RCC_AHB1ENR |= (1 << 1);   /* Enable GPIOB Clock */

    /* Set PB6 and PB7 to alternative function USART
     * See stm32f4_ref.pdf pg 141 and stm32f407.pdf pg 51 */

    /* Sets PB6 and PB7 to alternative function mode */
    *GPIOB_MODER &= ~((3 << (6 * 2)) | (3 << (7 * 2)));
    *GPIOB_MODER |= (GPIO_MODER_ALT << (6 * 2)) | (GPIO_MODER_ALT << (7 * 2));

    /* Sets PB6 and PB7 to USART1-3 mode */
    *GPIOB_AFRL  &= ~((0xF << (6 * 4)) | (0xF << (7 * 4)));
    *GPIOB_AFRL  |= (GPIO_AF_USART13 << (6 * 4)) | (GPIO_AF_USART13 << (7 * 4));

    /* Sets pin output to push/pull */
    *GPIOB_OTYPER &= ~((1 << 6) | (1 << 7));

    /* No pull-up, no pull-down */
    *GPIOB_PUPDR  &= ~((3 << (6 * 2)) | (3 << (7 * 2)));

    /* Speed to 50Mhz */
    *GPIOB_OSPEEDR &= ~((3 << (6 * 2)) | (3 << (7 * 2)));
    *GPIOB_OSPEEDR |= (2 << (6 * 2)) | (2 << (7 * 2));

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
    *DMA2_S2CR &= ~(DMA_SxCR_EN);
    *DMA2_S7CR &= ~(DMA_SxCR_EN);
    while ( (*DMA2_S2CR & DMA_SxCR_EN) || (*DMA2_S7CR & DMA_SxCR_EN) );

    /* Select channel 4 */
    *DMA2_S2CR |= DMA_SxCR_CHSEL(4);
    *DMA2_S7CR |= DMA_SxCR_CHSEL(4);

    /* Peripheral address - Both use USART data register */
    *DMA2_S2PAR = (uint32_t) USART1_DR;    /* RX */
    *DMA2_S7PAR = (uint32_t) USART1_DR;    /* TX */

    /* Allocate buffer memory */
    usart_rx_buf = (char *) malloc(USART_DMA_MSIZE);
    usart_tx_buf = (char *) malloc(USART_DMA_MSIZE);
    if ((usart_rx_buf == NULL) || (usart_tx_buf == NULL)) {
        panic();
    }
    else {
        /* Clear buffers */
        memset32(usart_rx_buf, 0, USART_DMA_MSIZE);
        memset32(usart_tx_buf, 0, USART_DMA_MSIZE);
        *DMA2_S2M0AR = (uint32_t) usart_rx_buf;
        *DMA2_S7M0AR = (uint32_t) usart_tx_buf;
    }

    /* Number of data items to be transferred */
    *DMA2_S2NDTR = (uint16_t) USART_DMA_MSIZE;

    /* FIFO setup */
    *DMA2_S7FCR |= DMA_SxFCR_FTH_4 | DMA_SxFCR_DMDIS;

    /* Data direct, memory increment, high priority, memory burst */
    *DMA2_S2CR |= DMA_SxCR_DIR_PM | DMA_SxCR_MINC | DMA_SxCR_PL_HIGH | DMA_SxCR_CIRC;
    *DMA2_S7CR |= DMA_SxCR_DIR_MP | DMA_SxCR_MINC | DMA_SxCR_PL_HIGH | DMA_SxCR_MBURST_4;

    /* Enable DMAs */
    *DMA2_S2CR |= DMA_SxCR_EN;

    /** DMA End **/

    /* Set baud rate */
    *USART1_BRR = usart_baud(115200);

    /* Enable reciever and transmitter */
    *USART1_CR1 |= USART_CR1_RE;
    *USART1_CR1 |= USART_CR1_TE;

    /* Reset semaphore */
    init_semaphore(&usart_semaphore);
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

void usart_puts(char *s) {
    acquire(&usart_semaphore);

    while (*s) {
        char *buf = usart_tx_buf;
        uint16_t count = 0;

        while (*s && count < USART_DMA_MSIZE) {
            count += 1;
            *buf++ = *s++;
        }

        /* Wait for DMA to be ready */
        while (*DMA2_S7CR & DMA_SxCR_EN) {
            if (task_switching) {
                _svc(SVC_YIELD);
            }
        }

        /* Number of bytes to write */
        *DMA2_S7NDTR = (uint16_t) count;
        /* Enable DMA */
        *DMA2_S7CR |= DMA_SxCR_EN;

        /* Wait for transfer to complete */
        while (!(*DMA2_HISR & DMA_HISR_TCIF7)) {
            if (task_switching) {
                _svc(SVC_YIELD);
            }
        }

        /* Clear transfer complete flag */
        *DMA2_HIFCR |= DMA_HIFCR_CTCIF7;

        /* Clear buffer */
        memset32(usart_tx_buf, 0, (count % 4) ? (count/4 + 1) : (count/4));
    }

    release(&usart_semaphore);
}

char usart_getc(void) {
    static uint32_t read = 0;
    uint16_t dma_read = USART_DMA_MSIZE - (uint16_t) *DMA2_S2NDTR;
    uint8_t wrapped = *DMA2_LISR & DMA_LISR_TCIF2;
    char *usart_buf = usart_rx_buf;

    while (!wrapped && dma_read == read) {
        /* Yield */
        _svc(1);

        dma_read = USART_DMA_MSIZE - (uint16_t) *DMA2_S2NDTR;
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
    /* The DMA has wrapped around, and has already caught up to us, start over
     * if (wrapped && dma_read >= read) */
    else {
        /* Clear completion flag */
        *DMA2_LIFCR |= DMA_LIFCR_CTCIF2;
        read = 1;
        return *usart_buf;
    }
}

void usart_echo(void) {
    char buf[17];
    uint16_t read = 0;
    uint16_t dma_read = USART_DMA_MSIZE - (uint16_t) *DMA2_S2NDTR;
    char *usart_buf = usart_rx_buf;

    buf[16] = '\0';

    /* Clear completion flag */
    *DMA2_LIFCR |= DMA_LIFCR_CTCIF2;

    while (1) {
        dma_read = USART_DMA_MSIZE - (uint16_t) *DMA2_S2NDTR;
        while ((read < dma_read) && !(*DMA2_LISR & DMA_LISR_TCIF2)) {
            uint8_t i = 0;
            uint8_t j = ((dma_read - read) > 16) ? 16 : (dma_read - read);
       
            for (i = 0; i < j; i++) {
                if (usart_buf >= (usart_rx_buf + USART_DMA_MSIZE)) {
                    buf[i] = '\0';
                }
                else {
                    buf[i] = *usart_buf++;
                }

                read++;
            }
            buf[i] = '\0';

            puts(buf);
            dma_read = USART_DMA_MSIZE - (uint16_t) *DMA2_S2NDTR;
        }

        /* Buffer has wrapped around */
        while (*DMA2_LISR & DMA_LISR_TCIF2) {
            *DMA2_LIFCR |= DMA_LIFCR_CTCIF2;

            /* Read to end of buffer */
            while (read < USART_DMA_MSIZE) {
                for (uint8_t i = 0; i < 16; i++) {
                    if (usart_buf >= (usart_rx_buf + USART_DMA_MSIZE)) {
                        buf[i] = '\0';
                    }
                    else {
                        buf[i] = *usart_buf++;
                    }

                    read++;
                }
                puts(buf);
            }

            read = 0;
            usart_buf = usart_rx_buf;
        }
    }
}
