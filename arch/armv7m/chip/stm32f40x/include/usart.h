/*
 * STM32F4 USART registers and bit fields
 */

#ifndef ARCH_CHIP_USART_H_INCLUDED
#define ARCH_CHIP_USART_H_INCLUDED

#include <arch/chip/registers.h>

typedef struct usart {
    volatile uint32_t SR;   // USARTx Status Register
    volatile uint32_t DR;   // USARTx Data Register
    volatile uint32_t BRR;  // USARTx Baud Rate Register
    volatile uint32_t CR1;  // USARTx Control Register 1
    volatile uint32_t CR2;  // USARTx Control Register 2
    volatile uint32_t CR3;  // USARTx Control Register 3
    volatile uint32_t GTPR; // USARTx Guard Time and Prescaler Register
} usart_t;

static usart_t * get_usart(int x) {
    switch(x) {
        case 1: return (usart_t *) USART1_BASE;
        case 2: return (usart_t *) USART2_BASE;
        case 3: return (usart_t *) USART3_BASE;
        case 4: return (usart_t *) UART4_BASE;
        case 5: return (usart_t *) UART5_BASE;
        case 6: return (usart_t *) USART6_BASE;
        case 7: return (usart_t *) UART7_BASE;
        case 8: return (usart_t *) UART8_BASE;
    }
    return (usart_t *) INVALID_PERIPH_BASE;
}

/* USARTx Status Register bit fields */
#define USART_SR_CTS                ((uint32_t) (1 << 9))           /* USART_SR CTS Flag */
#define USART_SR_LBD                ((uint32_t) (1 << 8))           /* USART_SR LIN break detection flag */
#define USART_SR_TXE                ((uint32_t) (1 << 7))           /* USART_SR Transmit data register empty */
#define USART_SR_TC                 ((uint32_t) (1 << 6))           /* USART_SR Transmission complete */
#define USART_SR_RXNE               ((uint32_t) (1 << 5))           /* USART_SR Read data register not empty */
#define USART_SR_IDLE               ((uint32_t) (1 << 4))           /* USART_SR IDLE line detected */
#define USART_SR_ORE                ((uint32_t) (1 << 3))           /* USART_SR Overrun error */
#define USART_SR_NF                 ((uint32_t) (1 << 2))           /* USART_SR Noise detected flag */
#define USART_SR_FE                 ((uint32_t) (1 << 1))           /* USART_SR Framing error */
#define USART_SR_PE                 ((uint32_t) (1 << 0))           /* USART_SR Parity error */

/* USARTx Control Register 1 bit fields */
#define USART_CR1_OVER8             ((uint32_t) (1 << 15))          /* USART_CR1 Oversampling mode */
#define USART_CR1_UE                ((uint32_t) (1 << 13))          /* USART_CR1 USART enable */
#define USART_CR1_M                 ((uint32_t) (1 << 12))          /* USART_CR1 Word length */
#define USART_CR1_WAKE              ((uint32_t) (1 << 11))          /* USART_CR1 Wakeup method */
#define USART_CR1_PCE               ((uint32_t) (1 << 10))          /* USART_CR1 Parity control enable */
#define USART_CR1_PS                ((uint32_t) (1 << 9))           /* USART_CR1 Parity selection */
#define USART_CR1_PEIE              ((uint32_t) (1 << 8))           /* USART_CR1 PE interrupt enable */
#define USART_CR1_TXEIE             ((uint32_t) (1 << 7))           /* USART_CR1 TXE interrupt enable */
#define USART_CR1_TCIE              ((uint32_t) (1 << 6))           /* USART_CR1 Transmission complete interrupt enable */
#define USART_CR1_RXNEIE            ((uint32_t) (1 << 5))           /* USART_CR1 RXNE interrupt enable */
#define USART_CR1_IDLEIE            ((uint32_t) (1 << 4))           /* USART_CR1 IDLE interrupt enable */
#define USART_CR1_TE                ((uint32_t) (1 << 3))           /* USART_CR1 Transmit enable */
#define USART_CR1_RE                ((uint32_t) (1 << 2))           /* USART_CR1 Receive Enable */
#define USART_CR1_RWU               ((uint32_t) (1 << 1))           /* USART_CR1 Receiver wakeup */
#define USART_CR1_SBK               ((uint32_t) (1 << 0))           /* USART_CR1 Send break */

/* USARTx Control Register 3 bit fields */
#define USART_CR3_ONEBIT            ((uint32_t) (1 << 11))          /* USART_CR3 One sample bit method enable */
#define USART_CR3_CTSIE             ((uint32_t) (1 << 10))          /* USART_CR3 CTS interrupt enable */
#define USART_CR3_CTSE              ((uint32_t) (1 << 9))           /* USART_CR3 CTS enable */
#define USART_CR3_RTSE              ((uint32_t) (1 << 8))           /* USART_CR3 RTS enable */
#define USART_CR3_DMAT              ((uint32_t) (1 << 7))           /* USART_CR3 DMA enable transmitter */
#define USART_CR3_DMAR              ((uint32_t) (1 << 6))           /* USART_CR3 DMA enable receiver */
#define USART_CR3_SCEN              ((uint32_t) (1 << 5))           /* USART_CR3 Smartcard mode enable */
#define USART_CR3_NACK              ((uint32_t) (1 << 4))           /* USART_CR3 Smartcard NACK enable */

#endif // DEV_USART_H_INCLUDED
