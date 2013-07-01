#include <stddef.h>
#include <stdlib.h>
#include <kernel/fault.h>
#include <mm/mm.h>
#include "buddy_mm_internals.h"

#ifdef CONFIG_MM_PROFILING
#include <dev/hw/perfcounter.h>
uint64_t begin_malloc_timestamp, end_malloc_timestamp;
#endif

static void *alloc(uint8_t order, struct buddy *buddy) __attribute__((section(".kernel")));
static struct heapnode *buddy_split(struct heapnode *node, struct buddy *buddy) __attribute__((section(".kernel")));
static uint8_t size_to_order(size_t size) __attribute__((section(".kernel")));

void *malloc(size_t size) {
    if(size > MM_MAX_USER_SIZE)
        return NULL;

    uint8_t order = size_to_order(size + MM_HEADER_SIZE);
    void *address;

    acquire(&user_buddy.semaphore);

#ifdef CONFIG_MM_PROFILING
    begin_malloc_timestamp = perfcounter_getcount();
#endif

    address = alloc(order, &user_buddy);

#ifdef CONFIG_MM_PROFILING
    end_malloc_timestamp = perfcounter_getcount();
#endif

    release(&user_buddy.semaphore);

    return address;
}

void *kmalloc(size_t size) {
    if(size > MM_MAX_KERNEL_SIZE)
        return NULL;

    uint8_t order = size_to_order(size + MM_HEADER_SIZE);
    void *address;

    acquire(&kernel_buddy.semaphore);
    address = alloc(order, &kernel_buddy);
    release(&kernel_buddy.semaphore);

    return address;
}

void *alloc(uint8_t order, struct buddy *buddy) {
    struct heapnode *node = NULL;

    if (order < buddy->min_order) {
        order = buddy->min_order;
    }
    else if (order > buddy->max_order) {
        return NULL;
    }

    /* Node available in this list */
    if (buddy->list[order] != NULL) {
        node = buddy->list[order];
        buddy->list[order] = buddy->list[order]->next;
    }
    else {
        uint8_t new_order = order;

        /* Find list with available nodes */
        while (node == NULL && (new_order+1) <= buddy->max_order) {
            node = buddy->list[++new_order];
        }

        if (node == NULL) {
            return NULL;
        }

        buddy->list[new_order] = buddy->list[new_order]->next;

        /* Split nodes down to size */
        while (new_order > order) {
            node = buddy_split(node, buddy);
            new_order--;
        }
    }

    if (node->header.magic != MM_MAGIC) {
        panic_print("mm: node with invalid magic found in buddy list. "
                    "buddy = 0x%x, node = 0x%x, node->header.magic = 0x%x, "
                    "node->header.order = %u", buddy, node, node->header.magic,
                    node->header.order);
    }

    if (node->header.order != order) {
        panic_print("mm: node->header.order != order, node: 0x%x "
                    "node->header.order: %d order: %d", node,
                    node->header.order, order);
    }

    return (void *) ((uint8_t *) node) + MM_HEADER_SIZE;
}

static struct heapnode *buddy_split(struct heapnode *node, struct buddy *buddy) {
    if (node->header.magic != MM_MAGIC) {
        panic_print("mm: attempting to split invalid node 0x%x", node);
    }

    uint8_t order = node->header.order;
    uint8_t new_order = order - 1;

    /* Create new node halfway through current node */
    struct heapnode *split_node = (struct heapnode *) ((uint8_t *) node + (1 << new_order));

    split_node->header.magic = MM_MAGIC;
    split_node->header.order = new_order;
    split_node->next = buddy->list[new_order];
    buddy->list[new_order] = split_node;

    node->header.order = new_order;
    node->next = NULL;

    return node;
}

static uint8_t size_to_order(size_t size) {
    uint8_t order = 0;

    /* If size isn't a power of two, round up */
    if (size & (size - 1)) {
        order = 1;
    }

    while (size >>= 1) {
        order += 1;
    }

    return order;
}
