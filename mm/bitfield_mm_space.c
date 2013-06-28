#include <stddef.h>
#include <stdint.h>
#include <kernel/semaphore.h>

#include "bitfield_mm_internals.h"

static uint32_t count_space(mm_block_t *heap, uint32_t hlen, struct semaphore *mutex) {
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
