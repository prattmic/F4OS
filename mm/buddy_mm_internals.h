#ifndef MM_BUDDY_MM_INTERNALS_INCLUDED
#define MM_BUDDY_MM_INTERNALS_INCLUDED

#include <kernel/semaphore.h>

#define MM_MAGIC    0xBEEF

struct heapnode_header {
    uint16_t magic;
    uint8_t order;
    uint8_t padding;    /* Tasks won't take kindly to */
                        /* getting unaligned addresses */
} __attribute__((packed));

struct heapnode {
    struct heapnode_header header;
    struct heapnode *next;
} __attribute__((packed));

struct buddy {
    uint8_t max_order;
    uint8_t min_order;
    struct semaphore semaphore;
    struct heapnode **list;
};

#define MM_HEADER_SIZE      sizeof(struct heapnode_header)
#define MM_MAX_USER_SIZE    (CONFIG_EUSERHEAP - CONFIG_SUSERHEAP)
#define MM_MAX_KERNEL_SIZE  (CONFIG_EKERNELHEAP - CONFIG_SKERNELHEAP)

extern struct buddy user_buddy;
extern struct heapnode *user_buddy_list[];

extern struct buddy kernel_buddy;
extern struct heapnode *kernel_buddy_list[];

#endif
