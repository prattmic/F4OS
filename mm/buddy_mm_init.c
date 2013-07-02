#include <stddef.h>
#include <stdint.h>

#include "buddy_mm_internals.h"

extern void *_suserheap;
extern void *_euserheap;
extern void *_skernelheap;
extern void *_ekernelheap;

struct buddy user_buddy;
/* Use one extra word so that we index with order directly, instead of order-1 */
struct heapnode *user_buddy_list[CONFIG_MM_USER_MAX_ORDER+1];

struct buddy kernel_buddy;
struct heapnode *kernel_buddy_list[CONFIG_MM_KERNEL_MAX_ORDER+1];

static void init_buddy(struct buddy *buddy, void *address) __attribute__((section(".kernel")));

void init_heap(void) {
    /* User buddy */
    user_buddy.max_order = CONFIG_MM_USER_MAX_ORDER;
    user_buddy.min_order = CONFIG_MM_USER_MIN_ORDER;
    init_semaphore(&user_buddy.semaphore);
    user_buddy.list = user_buddy_list;

    init_buddy(&user_buddy, &_suserheap);

    /* Kernel buddy */
    kernel_buddy.max_order = CONFIG_MM_KERNEL_MAX_ORDER;
    kernel_buddy.min_order = CONFIG_MM_KERNEL_MIN_ORDER;
    init_semaphore(&kernel_buddy.semaphore);
    kernel_buddy.list = kernel_buddy_list;

    init_buddy(&kernel_buddy, &_skernelheap);
}

static void init_buddy(struct buddy *buddy, void *address) {
    for (int i = 0; i < buddy->max_order; i++) {
        buddy->list[i] = NULL;
    }

    buddy->list[buddy->max_order] = (struct heapnode *) address;
    buddy->list[buddy->max_order]->header.magic = MM_MAGIC;
    buddy->list[buddy->max_order]->header.order = buddy->max_order;
    buddy->list[buddy->max_order]->next = NULL;
}
