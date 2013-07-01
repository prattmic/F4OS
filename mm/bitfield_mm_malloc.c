#include <stddef.h>
#include <stdint.h>
#include <kernel/semaphore.h>

#include "bitfield_mm_internals.h"

static void *alloc(mm_block_t *heap, uint32_t hlen, uint16_t grains, void *base, struct semaphore *mutex) {
    void *ret = NULL;
    uint32_t mask;
    uint32_t curr_free_blocks;
    uint32_t blocks_needed = 0;
    alloc_header_t *header = NULL;

    acquire(mutex);
    if(grains < MM_GRAINS_PER_BLOCK) {
        mask = MASK(grains);

        /* Go through blocks and look for enough free space */
        for(int i = 0; i <= hlen; i++) {
            if(heap[i].free_grains >= grains) {
                /* There is enough free space, now find where it is */
                for(int offset = 0; offset <= (MM_GRAINS_PER_BLOCK - grains); offset++) {
                    if(is_free(heap[i].free_mask, (mask << offset))) {
                        heap[i].free_mask |= (mask << offset);   /* Claim grains */
                        heap[i].free_grains -= grains;      /* Note change in block */
                        ret = to_addr(i, offset, base);          /* Get addr and leave */
                        goto out;
                    }
                }
            }
        }
    }
    else {
        curr_free_blocks = 0;
        blocks_needed = grains/MM_GRAINS_PER_BLOCK;
        grains = grains%MM_GRAINS_PER_BLOCK;
        mask = MASK(grains);

        /* Go through blocks and look for enough free space */
        for(int i = 0; i <= hlen; i++) {
            if(curr_free_blocks < blocks_needed) {
                if(!heap[i].free_mask) {
                    curr_free_blocks++;
                }
                else {
                    curr_free_blocks = 0;
                }
            }
            else if(curr_free_blocks == blocks_needed) {
                if(!(heap[i].free_mask & mask)) {
                    /* Claim all previous blocks */
                    for(int j = 1; j <= curr_free_blocks; j++) {
                        heap[i - j].free_mask = -1;     /* Set mask to all 1's */
                        heap[i - j].free_grains = 0;    /* No free grains left */
                    }
                    heap[i].free_mask |= mask;
                    heap[i].free_grains -= grains;                 /* Note change in block */
                    ret = to_addr(i-curr_free_blocks,  0, base);   /* Get addr and leave */
                    goto out;
                }
                else {
                    /* Space in partial block not free, start over */
                    curr_free_blocks = 0;
                }
            }
        }
    }

out:
    release(mutex);
    if(!ret)
        return ret;

    header = (alloc_header_t *)ret;
    header->magic = MM_MAGIC;
    header->grains = grains + MM_GRAINS_PER_BLOCK*blocks_needed;
    ret = (void *)((uintptr_t)ret + sizeof(alloc_header_t)); /* No arithmetic on void pointer... */
    return ret;
}

void *malloc(size_t size) {
    uint16_t grains;
    void *mem;

    if(size > MM_MAX_USER_SIZE)
        return NULL;

    size += sizeof(alloc_header_t);

    if(size > UINT16_MAX*MM_GRAINS_PER_BLOCK)
        return NULL;

    grains = size + MM_GRAIN_SIZE - 1;
    grains = grains/MM_GRAIN_SIZE;

    mem = alloc(userheap, MM_USER_NUM_BLOCKS, grains, (void *)&_suserheap, &userheap_mutex);
    return mem;
}

void *kmalloc(size_t size) {
    uint16_t grains;
    void *mem;

    if(size > MM_MAX_KERNEL_SIZE)
        return NULL;

    size += sizeof(alloc_header_t);

    if(size > UINT16_MAX*MM_GRAINS_PER_BLOCK)
        return NULL;

    grains = size + MM_GRAIN_SIZE - 1;
    grains = grains/MM_GRAIN_SIZE;

    mem = alloc(kernelheap, MM_KERNEL_NUM_BLOCKS, grains, (void *)&_skernelheap, &kernelheap_mutex);
    return mem;
}
