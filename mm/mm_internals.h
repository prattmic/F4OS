#ifndef MM_INTERNALS_INCLUDED
#define MM_INTERNALS_INCLUDED

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

struct buddy user_buddy;
struct heapnode *user_buddy_list[USER_MAX_ORDER+1];       /* Top is buddy_list[17], for locations 2^17 (128kb) in size */

struct buddy kernel_buddy;
struct heapnode *kernel_buddy_list[KERNEL_MAX_ORDER+1];

#endif
