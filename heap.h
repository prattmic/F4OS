#define HEAP_MEM_SIZE 8


/* User Heap */
void* malloc(int size, uint16_t aligned) __attribute__((section(".kernel")));
void init_heap(void)    __attribute__((section(".kernel")));

/* Kernel Heap */
void* kmalloc(int size) __attribute__((section(".kernel")));
void init_kheap(void)    __attribute__((section(".kernel")));
void* kmalloc_test(void) __attribute__((section(".kernel")));
