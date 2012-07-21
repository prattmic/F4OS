#include "types.h"
#include "registers.h"
#include "task.h"
#include "mem.h"
#include "semaphore.h"
#include "buddy.h"

extern uint32_t _suserheap;
extern uint32_t _euserheap;
extern uint32_t _skernelheap;
extern uint32_t _ekernelheap;

struct buddy user_buddy;
struct heapnode *user_buddy_list[USER_MAX_ORDER+1];       /* Top is buddy_list[17], for locations 2^17 (128kb) in size */

struct buddy kernel_buddy;
struct heapnode *kernel_buddy_list[KERNEL_MAX_ORDER+1];

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

void init_buddy(struct buddy *buddy, uint32_t *address) {
    for (int i = 0; i < buddy->max_order; i++) {
        buddy->list[i] = NULL;
    }

    buddy->list[buddy->max_order] = (struct heapnode *) address;
    buddy->list[buddy->max_order]->order = buddy->max_order;
    buddy->list[buddy->max_order]->next = NULL;
}

struct heapnode *buddy_split(struct heapnode *node, struct buddy *buddy) {
    /* Create new node */
    struct heapnode *split_node = (struct heapnode *) ((uint32_t) node + (1 << ((node->order % (buddy->max_order+1))-1)));

    split_node->order = node->order - 1;
    split_node->next = buddy->list[(node->order-1) % (buddy->max_order+1)];
    buddy->list[(node->order-1) % (buddy->max_order+1)] = split_node;

    node->order -= 1;
    node->next = NULL;

    return node;
}

uint8_t size_to_order(uint32_t size) {
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

void buddy_merge(struct heapnode *node, struct buddy *buddy) {
    struct heapnode *buddy_node = (struct heapnode *) ((uint32_t) node ^ (1 << node->order));     /* Note: this is not necessarily free */
    struct heapnode *curr_node = buddy->list[node->order];
    struct heapnode *prev_node = NULL;

    if (node->order >= buddy->max_order) {
        return;
    }

    /* Look for node and buddy */
    uint8_t found = 0;
    struct heapnode *node_curr_node = NULL;
    struct heapnode *node_prev_node = NULL;
    struct heapnode *buddy_curr_node = NULL;
    struct heapnode *buddy_prev_node = NULL;
    while (found < 2 && curr_node != NULL) {
        if (curr_node == node) {
            node_curr_node = curr_node;
            node_prev_node = prev_node;
            found += 1;
        }
        else if (curr_node == buddy_node) {
            buddy_curr_node = curr_node;
            buddy_prev_node = prev_node;
            found += 1;
        }

        prev_node = curr_node;
        curr_node = curr_node->next;
    }


    /* Buddy not free */
    if (buddy_curr_node == NULL) {
        if (node_curr_node == NULL) {
            node->next = buddy->list[node->order];
            buddy->list[node->order] = node;
        }
        return;
    }
    else {  /* Buddy free */
        /* Remove from list */
        if (buddy_prev_node == NULL) {
            buddy->list[node->order] = buddy_curr_node->next;
        }
        else {
            buddy_prev_node->next = buddy_curr_node->next;
        }

        /* Remove node if found */
        if (node_curr_node != NULL) {
            /* Remove from list */
            if (node_prev_node == NULL) {
                buddy->list[node->order] = node_curr_node->next;
            }
            else {
                node_prev_node->next = node_curr_node->next;
            }
        }

        node->order += 1;
        node->next = buddy->list[node->order];
        buddy->list[node->order] = node;

        /* Recurse */
        buddy_merge(node, buddy);
    }
}

void free(void *address) {
    struct heapnode *node = (struct heapnode *) ((uint32_t) address - BUDDY_HEADER_SIZE);

    acquire(&user_buddy.semaphore);
    buddy_merge(node, &user_buddy);
    release(&user_buddy.semaphore);
}

void kfree(void *address) {
    struct heapnode *node = (struct heapnode *) ((uint32_t) address - BUDDY_HEADER_SIZE);

    acquire(&kernel_buddy.semaphore);
    buddy_merge(node, &kernel_buddy);
    release(&kernel_buddy.semaphore);
}
