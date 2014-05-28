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

#include <stddef.h>
#include <stdint.h>
#include <kernel/mutex.h>

#include "bitfield_mm_internals.h"

static uint32_t count_space(mm_block_t *heap, uint32_t hlen, struct mutex *mutex) {
    uint32_t space = 0;

    acquire(mutex);
    for(int i = 0; i < hlen; i++) {
        space += MM_GRAIN_SIZE*heap[i].free_grains;
    }
    release(mutex);
    return space;
}

uint32_t mm_space(void) {
    return count_space(userheap, MM_USER_NUM_BLOCKS, &userheap_mutex);
}

uint32_t mm_kspace(void) {
    return count_space(kernelheap, MM_KERNEL_NUM_BLOCKS, &kernelheap_mutex);
}
