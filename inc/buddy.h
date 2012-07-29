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
extern struct buddy kernel_buddy;

void init_heap(void) __attribute__((section(".kernel")));
void init_buddy(struct buddy *buddy, uint32_t *address) __attribute__((section(".kernel")));
struct heapnode *buddy_split(struct heapnode *node, struct buddy *buddy) __attribute__((section(".kernel")));
uint8_t size_to_order(uint32_t size) __attribute__((section(".kernel")));
void *alloc(uint8_t order, struct buddy *buddy) __attribute__((section(".kernel")));
void *malloc(uint32_t size) __attribute__((section(".kernel")));
void *kmalloc(uint32_t size) __attribute__((section(".kernel")));
void buddy_merge(struct heapnode *node, struct buddy *buddy) __attribute__((section(".kernel")));
void free(void *address) __attribute__((section(".kernel")));
void kfree(void *address) __attribute__((section(".kernel")));
