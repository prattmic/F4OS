#include <stddef.h>
#include <stdlib.h>
#include <kernel/semaphore.h>

#include <mm/mm.h>
#include "mm_internals.h"

static void buddy_merge(struct heapnode *node, struct buddy *buddy) __attribute__((section(".kernel")));

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

        /* Set parent node as the less of the two buddies */
        node = node < buddy_curr_node ? node : buddy_curr_node;

        node->order += 1;
        node->next = buddy->list[node->order];
        buddy->list[node->order] = node;

        /* Recurse */
        buddy_merge(node, buddy);
    }
}

