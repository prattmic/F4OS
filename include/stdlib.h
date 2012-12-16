#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED

#include <stdint.h>

void *malloc(uint32_t size) __attribute__((malloc,section(".kernel")));
void free(void *address) __attribute__((section(".kernel")));
void abort(void) __attribute__((section(".kernel")));

inline int abs(int n) {
    return n > 0 ? n : -n;
}

#endif
