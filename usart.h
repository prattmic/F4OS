void init_usart(void) __attribute__((section(".kernel")));
uint16_t usart_baud(uint32_t baud) __attribute__((section(".kernel")));
void usart1_handler(void) __attribute__((section(".kernel")));
void printx(char* s, uint8_t*x, int n) __attribute__((section(".kernel")));
void putc(char letter) __attribute__((section(".kernel")));
void puts(char *s) __attribute__((section(".kernel")));
