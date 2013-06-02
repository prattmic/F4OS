#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED

#include <stddef.h>
#include <stdint.h>

void memset32(void *p, int32_t value, uint32_t size);
void memset(void *p, uint8_t value, uint32_t size);
void memcpy(void *dst, void *src, int n);
void memmove(void *dst, void *src, size_t n);
size_t strlen(char *s);
size_t strnlen(char *s, int n);
void strreverse(char *s);
int strncmp(char *s, char *p, uint32_t n);
char *strncpy(char *destination, char *source, int num);
int chrnlst(char c, char *l);

static inline int printable(char c) {
    return c >= 0x20 && c <= 0x7E;
}

#endif
