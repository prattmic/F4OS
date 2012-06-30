#include "types.h"
#include "registers.h"
#include "mem.h"
#include "buddy.h"

extern uint32_t _suserheap;
extern uint32_t _euserheap;
extern uint32_t _skernelheap;
extern uint32_t _ekernelheap;

void init_buddy(void) {
    for (int i = 0; i < MAX_ORDER; i++) {
        buddy_list[i] = NULL;
    }

    buddy_list[MAX_ORDER] = (struct heapnode *) &_suserheap;
    buddy_list[MAX_ORDER]->order = MAX_ORDER;
    buddy_list[MAX_ORDER]->next = NULL;
}

struct heapnode *buddy_split(struct heapnode *node) {
    /* Create new node */
    struct heapnode *split_node = (struct heapnode *) ((uint32_t) node + (1 << ((node->order % (MAX_ORDER+1))-1)));

    split_node->order = node->order - 1;
    split_node->next = buddy_list[(node->order-1) % (MAX_ORDER+1)];
    buddy_list[(node->order-1) % (MAX_ORDER+1)] = split_node;

    node->order -= 1;
    node->next = NULL;

    return node;
}

uint8_t size_to_order(uint32_t size) {
    uint8_t order = 0;

    while (size >>= 1) {
        order += 1;
    }

    return order;
}

void *malloc(uint32_t size) {
    uint8_t order = size_to_order(size + BUDDY_HEADER_SIZE);
    struct heapnode *node = NULL;

    if (order < MIN_ORDER) {
        order = MIN_ORDER;
    }
    else if (order > MAX_ORDER) {
        return NULL;
    }

    if (buddy_list[order] != NULL) {
        node = buddy_list[order];
        buddy_list[order] = buddy_list[order]->next;
        return ((uint32_t *) node) + BUDDY_HEADER_SIZE;
    }
    else {
        uint8_t new_order = order+1;

        while (node == NULL && new_order <= MAX_ORDER) {
            node = buddy_list[++new_order];
        }

        if (node == NULL) {
            return NULL;
        }

        buddy_list[new_order] = buddy_list[new_order]->next;

        while (new_order > order) {
            node = buddy_split(node);
            new_order--;
        }

        return (void *) ((uint32_t) node) + BUDDY_HEADER_SIZE;
    }
}

void buddy_merge(struct heapnode *node) {
    struct heapnode *buddy = (struct heapnode *) ((uint32_t) node ^ (1 << node->order));     /* Note: this is not necessarily free */
    struct heapnode *curr_node = buddy_list[node->order];
    struct heapnode *prev_node = NULL;

    if (node->order >= MAX_ORDER) {
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
        else if (curr_node == buddy) {
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
            node->next = buddy_list[node->order];
            buddy_list[node->order] = node;
        }
        return;
    }
    else {  /* Buddy free */
        /* Remove from list */
        if (buddy_prev_node == NULL) {
            buddy_list[node->order] = buddy_curr_node->next;
        }
        else {
            buddy_prev_node->next = buddy_curr_node->next;
        }

        /* Remove node if found */
        if (node_curr_node != NULL) {
            /* Remove from list */
            if (node_prev_node == NULL) {
                buddy_list[node->order] = node_curr_node->next;
            }
            else {
                node_prev_node->next = node_curr_node->next;
            }
        }

        node->order += 1;
        node->next = buddy_list[node->order];
        buddy_list[node->order] = node;

        /* Recurse */
        buddy_merge(node);
    }
}

void free(void *address) {
    struct heapnode *node = (struct heapnode *) ((uint32_t) address - BUDDY_HEADER_SIZE);

    buddy_merge(node);

    /* Well, that was fun */
}
