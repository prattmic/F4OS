#define MAX_ORDER   17
#define MIN_ORDER   4

struct heapnode {
    uint8_t order;
    struct heapnode *next;
};

struct heapnode *buddy_list[MAX_ORDER+1];       /* Top is buddy_list[17], for locations 2^17 (128kb) in size */

void init_buddy(void) __attribute__((section(".kernel")));
struct heapnode *buddy_split(struct heapnode *node) __attribute__((section(".kernel")));
uint8_t size_to_order(uint32_t size) __attribute__((section(".kernel")));
void *malloc(uint32_t size) __attribute__((section(".kernel")));
