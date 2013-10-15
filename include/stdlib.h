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

#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED

#include <stddef.h>
#include <stdint.h>

void *malloc(size_t size) __attribute__((malloc,section(".kernel")));
void free(void *address) __attribute__((section(".kernel")));
void abort(void) __attribute__((section(".kernel")));
char *strndup(char *str, int n);
int atoi(char buf[]);
char *itoa(int number, char *buf, uint32_t len, uint32_t base);
char *uitoa(uint32_t number, char *buf, uint32_t len, uint32_t base);
void ftoa(float num, float tolerance, char buf[], uint32_t n);

inline int abs(int n) {
    return n > 0 ? n : -n;
}

#endif
