#include <stdint.h>
#include <math.h>

#include <mm/mm.h>
#include "mm_internals.h"

static uint32_t free_memory(struct buddy *buddy);

uint32_t mm_space(void) {
    uint32_t space;
    acquire(&user_buddy.semaphore);
    space = free_memory(&user_buddy);
    release(&user_buddy.semaphore);

    return space;
}

uint32_t mm_kspace(void) {
    uint32_t space;
    acquire(&kernel_buddy.semaphore);
    space = free_memory(&kernel_buddy);
    release(&kernel_buddy.semaphore);

    return space;
}

static uint32_t free_memory(struct buddy *buddy) {
    uint32_t free = 0;

    for (int i = buddy->min_order; i <= buddy->max_order; i++) {
        struct heapnode *node = buddy->list[i];
        while (node) {
            free += pow(2, i);
            node = node->next;
        }
    }

    return free;
}
