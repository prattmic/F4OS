#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED

#include <stddef.h>
#include <stdint.h>

void *malloc(size_t size) __attribute__((malloc,section(".kernel")));
void free(void *address) __attribute__((section(".kernel")));
void abort(void) __attribute__((section(".kernel")));
char *strndup(char *str, int n);
int atoi(char buf[]);
char *itoa(int number, char *buf, uint32_t len, uint32_t base);
char *uitoa(uint32_t number, char *buf, uint32_t len, uint32_t base);
void ftoa(float num, float tolerance, char buf[], uint32_t n);

inline int abs(int n) {
    return n > 0 ? n : -n;
}

#endif
