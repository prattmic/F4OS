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
    uint8_t order = size_to_order(size + sizeof(uint8_t));
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
        return ((uint32_t *) node) + sizeof(uint8_t);
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

        return ((uint32_t *) node) + sizeof(uint8_t);
    }
}
