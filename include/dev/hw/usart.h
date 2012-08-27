#ifndef DEV_HW_USART_H_INCLUDED
#define DEV_HW_USART_H_INCLUDED

void init_usart1(void) __attribute__((section(".kernel")));
void usart_putc(char c, void *env) __attribute__((section(".kernel")));
void usart_puts(char *s, void *env) __attribute__((section(".kernel")));
char usart_getc(void *env) __attribute__((section(".kernel")));
void usart_close(resource *resource) __attribute__((section(".kernel")));

extern struct semaphore usart_semaphore;

#endif
