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
