#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED

#include <stdint.h>

void *malloc(uint32_t size) __attribute__((section(".kernel")));
void free(void *address) __attribute__((section(".kernel")));

#endif
