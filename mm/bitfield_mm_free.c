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
#include <kernel/fault.h>

#include "bitfield_mm_internals.h"

static void free_mem(void *mem, mm_block_t *heap, void *base, struct semaphore *mutex) {
    alloc_header_t *header = (alloc_header_t *)((uintptr_t)mem - sizeof(alloc_header_t));

    if(header->magic != MM_MAGIC)
        panic_print("Attempt to free corrupted or invalid heap object");

    uint16_t grains = header->grains;
    uint32_t idx = addr_to_block((void *)header, base);

    acquire(mutex);
    if(grains < MM_GRAINS_PER_BLOCK) {
        heap[idx].free_mask &= ~(MASK(grains) << addr_to_grain_offset((void *)header, base));
        heap[idx].free_grains += grains;
    }
    else {
        uint32_t blocks_to_free = grains/MM_GRAINS_PER_BLOCK;
        grains = grains%MM_GRAINS_PER_BLOCK;

        for(int i = 0; i < blocks_to_free; i++) {
            heap[idx+i].free_mask = 0;
            heap[idx+i].free_grains = MM_GRAINS_PER_BLOCK;
        }

        heap[idx+blocks_to_free].free_mask &= ~MASK(grains);
        heap[idx+blocks_to_free].free_grains += grains;
    }
    release(mutex);
}

void free(void *mem) {
    free_mem(mem, userheap, &_suserheap, &userheap_mutex);
}

void kfree(void *mem) {
    free_mem(mem, kernelheap, &_skernelheap, &kernelheap_mutex);
}
