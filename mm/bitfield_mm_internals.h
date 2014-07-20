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

#ifndef MM_BITFIELD_MM_INTERNALS_H_INCLUDED
#define MM_BITFIELD_MM_INTERNALS_H_INCLUDED

#define MM_GRAINS_PER_BLOCK 32          /* Due to bits in uint32_t */
#define MM_GRAIN_SIZE (1 << CONFIG_MM_GRAIN_SHIFT)
#define MM_MAGIC 0xABCD
#define MM_BLOCK_SIZE (MM_GRAINS_PER_BLOCK*MM_GRAIN_SIZE)
#define MM_USER_NUM_BLOCKS ((CONFIG_EUSERHEAP - CONFIG_SUSERHEAP)/MM_BLOCK_SIZE)
#define MM_KERNEL_NUM_BLOCKS ((CONFIG_EKERNELHEAP - CONFIG_SKERNELHEAP)/MM_BLOCK_SIZE)
#define MM_MAX_USER_SIZE (CONFIG_EUSERHEAP - CONFIG_SUSERHEAP)
#define MM_MAX_KERNEL_SIZE (CONFIG_EKERNELHEAP - CONFIG_SKERNELHEAP)

/* Gives back mask with n bits set to 1 */
#define MASK(n) ((unsigned int)((1UL << (n)) - 1))

typedef struct alloc_header {
    uint16_t    magic;
    uint16_t    grains;
} alloc_header_t;

typedef struct mm_block {
    uint8_t     free_grains;
    uint32_t    free_mask;
} mm_block_t;

extern mm_block_t userheap[];
extern mm_block_t kernelheap[];
extern struct mutex userheap_mutex;
extern struct mutex kernelheap_mutex;

/* Returns value to left shift mask by to look at freemask */
static inline uint32_t addr_to_grain_offset(void *addr, void *base) {
    return (((uintptr_t)addr - (uintptr_t)base)%MM_BLOCK_SIZE)/MM_GRAIN_SIZE;
}

/* Returns whether or not space for mask is free in a freemask */
static inline int is_free(uint32_t field, uint32_t mask) {
    return ((field & mask) == 0);
}

/* Returns block index given address */
static inline uint32_t addr_to_block(void *addr, void *base) {
    return ((uintptr_t)addr - (uintptr_t)base)/MM_BLOCK_SIZE;
}

/* Returns address of allocation to return */
static inline void *to_addr(uint32_t idx, uint32_t grain_offset, void *base) {
    return (void *)((uintptr_t)base + idx*MM_BLOCK_SIZE + grain_offset*MM_GRAIN_SIZE);
}

#endif
