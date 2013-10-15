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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <mm/mm.h>
#include "test.h"
#include <limits.h>

int malloc_simple(char *message, int len) {
    void *mem = malloc(1024);
    if (!mem) {
        return FAILED;
    }

    free(mem);
    return PASSED;
}
DEFINE_TEST("Simple malloc", malloc_simple);

int kmalloc_simple(char *message, int len) {
    void *mem = kmalloc(1024);
    if (!mem) {
        return FAILED;
    }

    kfree(mem);
    return PASSED;
}
DEFINE_TEST("Simple kmalloc", kmalloc_simple);

int malloc_range(char *message, int len) {
    uint32_t sizes[] = {1, 4, 8, 16, 32, 64, 128,
        256, 512, 1024, 2048, 4096, 8192, 16384,
        37, 353, 7894, 10000, 2560};

    for (int i = 0; i < ARRAY_LENGTH(sizes); i++) {
        void *mem = malloc(sizes[i]);
        if (!mem) {
            scnprintf(message, len, "Allocation of %d bytes failed", sizes[i]);
            return FAILED;
        }

        free(mem);
    }

    return PASSED;
}
DEFINE_TEST("malloc range", malloc_range);

int kmalloc_range(char *message, int len) {
    uint32_t sizes[] = {1, 4, 8, 16, 32, 64, 128,
        256, 512, 1024, 2048, 4096, 8192, 37, 353,
        7894, 1000, 2560};

    for (int i = 0; i < ARRAY_LENGTH(sizes); i++) {
        void *mem = kmalloc(sizes[i]);
        if (!mem) {
            scnprintf(message, len, "Allocation of %d bytes failed", sizes[i]);
            return FAILED;
        }

        kfree(mem);
    }

    return PASSED;
}
DEFINE_TEST("kmalloc range", kmalloc_range);

int malloc_toobig(char *message, int len) {
    void *mem = malloc(UINT_MAX);
    if(mem) {
        scnprintf(message, len, "Malloc of too much memory did not return NULL");
        free(mem);  /* wat */
        return FAILED;
    }

    return PASSED;
}
DEFINE_TEST("malloc too big", malloc_toobig);

int kmalloc_toobig(char *message, int len) {
    void *mem = kmalloc(UINT_MAX);
    if(mem) {
        scnprintf(message, len, "Malloc of too much memory did not return NULL");
        kfree(mem);  /* wat */
        return FAILED;
    }

    return PASSED;
}
DEFINE_TEST("kmalloc too big", kmalloc_toobig);
