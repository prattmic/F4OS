#include <stddef.h>
#include <stdint.h>

#include "mm_internals.h"

extern uint32_t _suserheap;
extern uint32_t _euserheap;
extern uint32_t _skernelheap;
extern uint32_t _ekernelheap;

struct buddy user_buddy;
struct heapnode *user_buddy_list[USER_MAX_ORDER+1];       /* Top is buddy_list[17], for locations 2^17 (128kb) in size */

struct buddy kernel_buddy;
struct heapnode *kernel_buddy_list[KERNEL_MAX_ORDER+1];

static void init_buddy(struct buddy *buddy, uint32_t *address) __attribute__((section(".kernel")));

void init_heap(void) {
    /* User buddy */
    user_buddy.max_order = USER_MAX_ORDER;
    user_buddy.min_order = USER_MIN_ORDER;
    init_semaphore(&user_buddy.semaphore);
    user_buddy.list = user_buddy_list;

    init_buddy(&user_buddy, &_suserheap);

    /* Kernel buddy */
    kernel_buddy.max_order = KERNEL_MAX_ORDER;
    kernel_buddy.min_order = KERNEL_MIN_ORDER;
    init_semaphore(&kernel_buddy.semaphore);
    kernel_buddy.list = kernel_buddy_list;

    init_buddy(&kernel_buddy, &_skernelheap);
}

static void init_buddy(struct buddy *buddy, uint32_t *address) {
    for (int i = 0; i < buddy->max_order; i++) {
        buddy->list[i] = NULL;
    }

    buddy->list[buddy->max_order] = (struct heapnode *) address;
    buddy->list[buddy->max_order]->order = buddy->max_order;
    buddy->list[buddy->max_order]->next = NULL;
}
