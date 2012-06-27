#define HEAP_MEM_SIZE 8


/* User Heap */
void* malloc(uint32_t size, uint16_t aligned) __attribute__((section(".kernel")));
void free(void *mem, uint32_t size) __attribute__((section(".kernel")));
void init_uheap(void)    __attribute__((section(".kernel")));

/* Kernel Heap */
void init_kheap(void)    __attribute__((section(".kernel")));
void* kmalloc(uint32_t size) __attribute__((section(".kernel")));
void kfree(void *mem, uint32_t size) __attribute__((section(".kernel")));
