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

#include <stddef.h>
#include <stdint.h>
#include <kernel/semaphore.h>

#include "bitfield_mm_internals.h"

struct semaphore userheap_mutex = INIT_SEMAPHORE;
struct semaphore kernelheap_mutex = INIT_SEMAPHORE;

mm_block_t userheap[MM_USER_NUM_BLOCKS];
mm_block_t kernelheap[MM_KERNEL_NUM_BLOCKS];

static void init_this_heap(mm_block_t *heap, uint32_t size) {
    for(int i = 0; i < size; i++) {
        heap[i].free_grains = MM_GRAINS_PER_BLOCK;
        heap[i].free_mask = 0;
    }
}

void init_heap(void) {
    init_this_heap(kernelheap, MM_KERNEL_NUM_BLOCKS);
    init_this_heap(userheap, MM_USER_NUM_BLOCKS);
}
