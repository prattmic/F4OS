#ifndef DEV_HW_USART_H_INCLUDED
#define DEV_HW_USART_H_INCLUDED

void init_usart(void) __attribute__((section(".kernel")));
int usart_putc(char c, void *env) __attribute__((section(".kernel")));
int usart_puts(char *s, void *env) __attribute__((section(".kernel")));
char usart_getc(void *env, int *error) __attribute__((section(".kernel")));
int usart_close(resource *resource) __attribute__((section(".kernel")));

extern uint8_t usart_ready;

#endif
