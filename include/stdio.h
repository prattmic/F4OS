#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED

#include <stdint.h>

typedef uint8_t rd_t;

void write(rd_t rd, char *d, int n) __attribute__((section(".kernel")));
void close(rd_t rd) __attribute__((section(".kernel")));
void read(rd_t rd, char *buf, int n) __attribute__((section(".kernel")));
void swrite(rd_t rd, char *s) __attribute__((section(".kernel")));

void fputs(rd_t rd, char *s);
void fputc(rd_t rd, char letter);
char fgetc(rd_t rd);
void fprintf(rd_t rd, char *fmt, ...);

void puts(char *s);
void putc(char letter);
char getc(void);
void printx(char *s, uint8_t *x, int n);
#define printf(args...) fprintf(0, args)

#endif
