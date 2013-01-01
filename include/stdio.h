#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED

#include <stdint.h>
#include <stdarg.h>

typedef int8_t rd_t;

#define stdin   0
#define stdout  0
#define stderr  1

int write(rd_t rd, char *d, int n) __attribute__((section(".kernel")));
int close(rd_t rd) __attribute__((section(".kernel")));
int read(rd_t rd, char *buf, int n) __attribute__((section(".kernel")));
int swrite(rd_t rd, char *s) __attribute__((section(".kernel")));

int sprintf(char *buf, char *fmt, ...);

int fputs(rd_t rd, char *s);
int fputc(rd_t rd, char letter);
int fgetc(rd_t rd);
int fprintf(rd_t rd, char *fmt, ...);
int vfprintf(rd_t rd, char *fmt, va_list ap, int (*puts_fn)(rd_t,char*), int (*putc_fn)(rd_t,char));

int puts(char *s);
int putc(char letter);
#define getc() fgetc(stdin)
#define printf(args...) fprintf(stdout, args)

void printx(char *s, uint8_t *x, int n);

#endif
