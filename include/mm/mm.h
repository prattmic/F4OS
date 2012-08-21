#ifndef MM_MM_H_INCLUDED
#define MM_MM_H_INCLUDED

void init_heap(void) __attribute__((section(".kernel")));
void *kmalloc(uint32_t size) __attribute__((section(".kernel")));
void kfree(void *address) __attribute__((section(".kernel")));

#endif
