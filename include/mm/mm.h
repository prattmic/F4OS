#ifndef MM_MM_H_INCLUDED
#define MM_MM_H_INCLUDED

#include <stddef.h>
#include <stdint.h>

void init_heap(void) __attribute__((section(".kernel")));
void *kmalloc(size_t size) __attribute__((malloc,section(".kernel")));
void kfree(void *address) __attribute__((section(".kernel")));

uint32_t mm_space(void) __attribute__((section(".kernel")));
uint32_t mm_kspace(void) __attribute__((section(".kernel")));

#ifdef CONFIG_MM_PROFILING
extern uint64_t begin_malloc_timestamp, end_malloc_timestamp;
#endif

#endif
