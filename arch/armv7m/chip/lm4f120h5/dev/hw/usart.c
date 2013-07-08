#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arch/system.h>
#include <arch/chip/registers.h>
#include <arch/chip/rom.h>
#include <dev/resource.h>
#include <kernel/sched.h>
#include <kernel/semaphore.h>
#include <kernel/fault.h>

#include <dev/hw/usart.h>

struct semaphore usart_read_semaphore = INIT_SEMAPHORE;
struct semaphore usart_write_semaphore = INIT_SEMAPHORE;

resource usart_resource = { .writer     = &usart_putc,
                            .swriter    = &usart_puts,
                            .reader     = &usart_getc,
                            .closer     = &usart_close,
                            .env        = NULL,
                            .read_sem   = &usart_read_semaphore,
                            .write_sem  = &usart_write_semaphore};

static void usart_baud(uint32_t baud) __attribute__((section(".kernel")));

uint8_t usart_ready = 0;

void init_usart(void) {
    /* Enable UART0 clock */
    SYSCTL_RCGC1_R |= SYSCTL_RCGC1_UART0;

    /* Enable GPIOA clock */
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;

    ROM_SysCtlDelay(1);

    /* Alternate functions UART */
    GPIO_PORTA_PCTL_R |= (1 << 4) | (1 << 0);

    /* Set to output */
    GPIO_PORTA_DIR_R &= ~(GPIO_PIN_1 | GPIO_PIN_0);

    /* Set PA0 and PA1 to alternate function */
    GPIO_PORTA_AFSEL_R |= GPIO_PIN_1 | GPIO_PIN_0;

    /* Digital pins */
    GPIO_PORTA_DEN_R |= GPIO_PIN_1 | GPIO_PIN_0;

    /* Disable UART */
    UART0_CTL_R &= ~(UART_CTL_UARTEN);

    usart_baud(115200);

    /* Enable FIFO, 8-bit words */
    UART0_LCRH_R = UART_LCRH_FEN | UART_LCRH_WLEN_8;

    /* Use system clock */
    UART0_CC_R = UART_CC_CS_SYSCLK;

    /* Enable UART */
    UART0_CTL_R |= UART_CTL_UARTEN | UART_CTL_RXE | UART_CTL_TXE;

    usart_ready = 1;
}


/* Sets baud rate registers */
void usart_baud(uint32_t baud) {
    float brd = ROM_SysCtlClockGet() / (16 * baud);
    int brdi = (int) brd;
    int brdf = (int) (brd * 64 + 0.5);

    UART0_IBRD_R = brdi;
    UART0_FBRD_R = brdf;
}

int usart_putc(char c, void *env) {
    /* Wait until transmit FIFO not full*/
    while (UART0_FR_R & UART_FR_TXFF) {
        yield_if_possible();
    }

    UART0_DR_R = c;

    return 1;
}

int usart_puts(char *s, void *env) {
    int total = 0;
    while (*s) {
        int ret = usart_putc(*s++, env);
        if (ret > 0) {
            total += ret;
        }
        else {
            total = ret;
            break;
        }
    }

    return total;
}

char usart_getc(void *env, int *error) {
    if (error != NULL) {
        *error = 0;
    }

    /* Wait for data */
    while (UART0_FR_R & UART_FR_RXFE) {
        yield_if_possible();
    }

    return UART0_DR_R & UART_DR_DATA_M;
}

int usart_close(resource *resource) {
    printk("OOPS: USART is a fundamental resource, it may not be closed.");
    return -1;
}
