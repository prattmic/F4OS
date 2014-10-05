/*
 * Copyright (C) 2013, 2014 F4OS Authors
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
#include <string.h>
#include <dev/char.h>
#include <kernel/sched.h>

#define stdin   (curr_task->_stdin)
#define stdout  (curr_task->_stdout)
#define stderr  (curr_task->_stderr)

int read(struct char_device *dev, char *buf, int num);
int write(struct char_device *dev, const char *buf, int num);

/*
 * read() and write() variants which block until all bytes are
 * read/written, or an error occurs.
 */
int read_block(struct char_device *dev, char *buf, int num);
int write_block(struct char_device *dev, const char *buf, int num);

/* Print fmt into buf, writing at most n bytes.
 * Returns number of characters written to buffer. */
int scnprintf(char *buf, uint32_t n, const char *fmt, ...);

int fputs(struct char_device *dev, const char *s);
int fputc(struct char_device *dev, const char letter);
int fgetc(struct char_device *dev);
int fprintf(struct char_device *dev, const char *fmt, ...);
int vfprintf(struct char_device *dev, const char *fmt, va_list ap);

#define puts(s) fputs(stdout, s)
#define putc(c) fputc(stdout, c)
#define getc() fgetc(stdin)
#define printf(args...) fprintf(stdout, args)

#endif
