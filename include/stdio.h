/*
 * Copyright (C) 2013 F4OS Authors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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

/* Print fmt into buf, writing at most n bytes.
 * Returns number of characters written to buffer. */
int scnprintf(char *buf, uint32_t n, char *fmt, ...);

int fputs(rd_t rd, char *s);
int fputc(rd_t rd, char letter);
int fgetc(rd_t rd);
int fprintf(rd_t rd, char *fmt, ...);
int vfprintf(rd_t rd, char *fmt, va_list ap, int (*puts_fn)(rd_t,char*), int (*putc_fn)(rd_t,char));

#define puts(s) fputs(stdout, s)
#define putc(c) fputc(stdout, c)
#define getc() fgetc(stdin)
#define printf(args...) fprintf(stdout, args)

#endif
