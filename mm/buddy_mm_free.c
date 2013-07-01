#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <kernel/semaphore.h>
#include <kernel/fault.h>

#include <mm/mm.h>
#include "buddy_mm_internals.h"

static void buddy_merge(struct heapnode *node, struct buddy *buddy) __attribute__((section(".kernel")));

void free(void *address) {
    struct heapnode *node = (struct heapnode *) ((uint8_t *) address - MM_HEADER_SIZE);

    acquire(&user_buddy.semaphore);
    buddy_merge(node, &user_buddy);
    release(&user_buddy.semaphore);
}

void kfree(void *address) {
    struct heapnode *node = (struct heapnode *) ((uint8_t *) address - MM_HEADER_SIZE);

    acquire(&kernel_buddy.semaphore);
    buddy_merge(node, &kernel_buddy);
    release(&kernel_buddy.semaphore);
}

void buddy_merge(struct heapnode *node, struct buddy *buddy) {
    if (node->header.magic != MM_MAGIC) {
        fprintf(stderr, "OOPS: mm: attempted to merge invalid node 0x%x\r\n", node);
        return;
    }

    uint8_t order = node->header.order;

    /* There is only one node of maximum size */
    if (order == buddy->max_order) {
        buddy->list[buddy->max_order] = node;
        node->next = NULL;
        return;
    }

    /* Our buddy node covers the other half of this order of memory,
     * thus it will have the order bit in the opposite state of ours.
     * Note: this is not necessarily free */
    struct heapnode *buddy_node = (struct heapnode *) ((uintptr_t) node ^ (1 << order));
    struct heapnode *curr_node = buddy->list[order];
    struct heapnode *prev_node = NULL;

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
        /* If node already in list, leave it,
         * otherwise add it */
        if (node_curr_node == NULL) {
            node->next = buddy->list[order];
            buddy->list[order] = node;
        }
        return;
    }
    else {  /* Buddy free */
        if (buddy_node->header.order != order) {
            panic_print("mm: node->header.order != buddy_node->header.order, "
                        "node: 0x%x node->header.order: %d buddy_node: 0x%x, "
                        "buddy_node->header.order: %d", node, node->header.order,
                        buddy_node, buddy_node->header.order);
        }

        /* Remove buddy from list */
        if (buddy_prev_node == NULL) {
            buddy->list[order] = buddy_curr_node->next;
        }
        else {
            buddy_prev_node->next = buddy_curr_node->next;
        }

        /* Remove node if found */
        if (node_curr_node != NULL) {
            /* Remove node from list */
            if (node_prev_node == NULL) {
                buddy->list[order] = node_curr_node->next;
            }
            else {
                node_prev_node->next = node_curr_node->next;
            }
        }

        /* Set parent node as the less of the two buddies */
        node = node < buddy_curr_node ? node : buddy_curr_node;

        /* Merge the nodes simply by increasing the order
         * of the smaller node. */
        uint8_t new_order = order + 1;
        node->header.order = new_order;

        /* Put on higher order list */
        node->next = buddy->list[new_order];
        buddy->list[new_order] = node;

        /* Recurse */
        buddy_merge(node, buddy);
    }
}
