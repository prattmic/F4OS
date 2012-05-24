
#define HEAP_MEM_SIZE 8


void* kmalloc(int size) __attribute__((section(".kernel")));
void* malloc_test(void) __attribute__((section(".kernel")));
void init_kheap(void)    __attribute__((section(".kernel")));
