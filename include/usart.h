#define USART_DMA_MSIZE     (512-sizeof(uint32_t))      /* Since malloc headers take up some space, we want to request the max space we can fit in one block */

void init_usart(void) __attribute__((section(".kernel")));
uint16_t usart_baud(uint32_t baud) __attribute__((section(".kernel")));
void usart1_handler(void) __attribute__((section(".kernel")));
void usart_putc(char c, void *env) __attribute__((section(".kernel")));
void usart_puts(char *s, void *env) __attribute__((section(".kernel")));
char usart_getc(void *env) __attribute__((section(".kernel")));
void usart_echo(void) __attribute__((section(".kernel")));

volatile struct semaphore usart_semaphore;

char *usart_rx_buf;
char *usart_tx_buf;
