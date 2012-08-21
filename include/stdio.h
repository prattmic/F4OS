#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED

#include <stdint.h>

typedef uint8_t rd_t;

void write(rd_t rd, char *d, int n) __attribute__((section(".kernel")));
void close(rd_t rd) __attribute__((section(".kernel")));
void read(rd_t rd, char *buf, int n) __attribute__((section(".kernel")));
void swrite(rd_t rd, char *s) __attribute__((section(".kernel")));

void puts(char *s);
char getc(void);
void putc(char letter);
void printx(char *s, uint8_t *x, int n);
void printf(char *fmt, ...);

#endif
