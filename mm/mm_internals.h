#ifndef MM_INTERNALS_INCLUDED
#define MM_INTERNALS_INCLUDED

#include <kernel/semaphore.h>

#define USER_MAX_ORDER   17
#define USER_MIN_ORDER   4

#define KERNEL_MAX_ORDER   15
#define KERNEL_MIN_ORDER   4

#define BUDDY_HEADER_SIZE   sizeof(uint32_t)

struct heapnode {
    uint8_t order;
    struct heapnode *next;
};

struct buddy {
    uint8_t max_order;
    uint8_t min_order;
    struct semaphore semaphore;
    struct heapnode **list;
};

extern struct buddy user_buddy;
extern struct heapnode *user_buddy_list[];       /* Top is buddy_list[17], for locations 2^17 (128kb) in size */

extern struct buddy kernel_buddy;
extern struct heapnode *kernel_buddy_list[];

#endif
