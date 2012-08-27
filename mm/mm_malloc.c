#include <stddef.h>
#include <stdlib.h>
#include <mm/mm.h>
#include "mm_internals.h"

static void *alloc(uint8_t order, struct buddy *buddy) __attribute__((section(".kernel")));
static struct heapnode *buddy_split(struct heapnode *node, struct buddy *buddy) __attribute__((section(".kernel")));
static uint8_t size_to_order(uint32_t size) __attribute__((section(".kernel")));

void *malloc(uint32_t size) {
    uint8_t order = size_to_order(size + BUDDY_HEADER_SIZE);
    void *address;

    acquire(&user_buddy.semaphore);
    address = alloc(order, &user_buddy);
    release(&user_buddy.semaphore);

    return address;
}

void *kmalloc(uint32_t size) {
    uint8_t order = size_to_order(size + BUDDY_HEADER_SIZE);
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

    if (buddy->list[order] != NULL) {
        node = buddy->list[order];
        buddy->list[order] = buddy->list[order]->next;
        return (void *) ((uint32_t) node) + BUDDY_HEADER_SIZE;
    }
    else {
        uint8_t new_order = order;

        while (node == NULL && (new_order+1) <= buddy->max_order) {
            node = buddy->list[++new_order];
        }

        if (node == NULL) {
            return NULL;
        }

        buddy->list[new_order] = buddy->list[new_order]->next;

        while (new_order > order) {
            node = buddy_split(node, buddy);
            new_order--;
        }

        return (void *) ((uint32_t) node) + BUDDY_HEADER_SIZE;
    }
}

static struct heapnode *buddy_split(struct heapnode *node, struct buddy *buddy) {
    /* Create new node */
    struct heapnode *split_node = (struct heapnode *) ((uint32_t) node + (1 << ((node->order % (buddy->max_order+1))-1)));

    split_node->order = node->order - 1;
    split_node->next = buddy->list[(node->order-1) % (buddy->max_order+1)];
    buddy->list[(node->order-1) % (buddy->max_order+1)] = split_node;

    node->order -= 1;
    node->next = NULL;

    return node;
}

static uint8_t size_to_order(uint32_t size) {
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
