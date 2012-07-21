#define USART_DMA_MSIZE     (512-sizeof(uint32_t))      /* Since malloc headers take up some space, we want to request the max space we can fit in one block */

void init_usart(void) __attribute__((section(".kernel")));
uint16_t usart_baud(uint32_t baud) __attribute__((section(".kernel")));
void usart1_handler(void) __attribute__((section(".kernel")));
void printx(char *s, uint8_t *x, int n) __attribute__((section(".kernel")));
void printf(char *fmt, ...) __attribute__((section(".kernel")));
void putc(char letter) __attribute__((section(".kernel")));
void puts(char *s) __attribute__((section(".kernel")));
char getc(void) __attribute__((section(".kernel")));
void usart_echo(void) __attribute__((section(".kernel")));

volatile struct semaphore usart_semaphore;

char *usart_rx_buf;
char *usart_tx_buf;
