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

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <mm/mm.h>
#include <math.h>
#include "app.h"

#define ITERATIONS 10

void mem_perf(int argc, char **argv) {
    uint32_t times[ITERATIONS];
    uint32_t total;

    printf("ALLOCATOR BENCHMARKS\r\n");

    for(int n = CONFIG_MM_USER_MIN_ORDER; n < CONFIG_MM_USER_MAX_ORDER; n++) {
        total = 0;
        uint32_t blocksize = pow(2, n);
        printf("Basic alloc %d bytes\r\n", blocksize);
        for(int i = 0; i < ITERATIONS; i++) {
            void *stuff = malloc(blocksize);
            if (!stuff) {
                printf("Warning: no memory available\r\n");
            }
            else {
                free(stuff);
            }

            times[i] = (uint32_t)(end_malloc_timestamp - begin_malloc_timestamp);
            total += times[i];
            #ifdef VERBOSE
            printf("--Time Delta %d = %u\r\n", i, times[i]);
            #endif
        }
        printf("--Average time %fus\r\n", (total/((float)ITERATIONS))/(CONFIG_SYS_CLOCK/1e6));
    }
}
DEFINE_APP(mem_perf)
