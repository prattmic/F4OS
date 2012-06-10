void init_usart(void) __attribute__((section(".kernel")));
uint16_t usart_baud(uint32_t baud) __attribute__((section(".kernel")));
void usart_read(void) __attribute__((section(".kernel")));
void putc(char letter) __attribute__((section(".kernel")));
void puts(char *s) __attribute__((section(".kernel")));
