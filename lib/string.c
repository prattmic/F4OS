#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/fault.h>

/* Set size bytes to value from p */
void memset32(void *p, int32_t value, uint32_t size) {
    uint32_t *end = (uint32_t *) ((uintptr_t) p + size);

    /* Disallowed unaligned addresses */
    if ( (uintptr_t) p % 4 ) {
        panic_print("Attempt to memset unaligned address (0x%x).", p);
    }

    while ( (uint32_t*) p < end ) {
        *((uint32_t*)p) = value;
        p++;
    }
}

/* Set size bytes to value from p */
void memset(void *p, uint8_t value, uint32_t size) {
    uint8_t *end = (uint8_t *) ((uintptr_t) p + size);

    while ( (uint8_t*) p < end ) {
        *((uint8_t*)p) = value;
        p++;
    }
}

void memcpy(void *dst, void *src, int n) {
    uint8_t *s = src;
    uint8_t *d = dst;

    while(n--) {
        *d++ = *s++;
    }
}

// Overlap-safe memcpy
void memmove(void *dst, void *src, size_t n) {
    char *s = src;
    char *d = dst;
    if ((uintptr_t)s < (uintptr_t)d)
        while(n--) d[n] = s[n];
    else
        while(n--) *d++ = *s++;
}

size_t strlen(char *s) {
    size_t len = 0;
    while (*s++) {
        len++;
    }
    return len;
}

size_t strnlen(char *s, int n) {
    size_t len = 0;
    while (*s++ && len < n) {
        len++;
    }
    return len;
}

void strreverse(char *s) {
    char *begin = s;

    if (!s || !*s) {
        return;
    }

    while (*++s);
    s--;

    while (s > begin) {
        char temp = *begin;
        *begin++ = *s;
        *s-- = temp;
    }
}

int strncmp(char *s, char *p, uint32_t n) {
    while (*s == *p && *s != '\0' && *p != '\0' && n) {
        s++;
        p++;
        n--;
    }

    if (!n || *s == *p) {
        return 0;
    }
    else if (*s > *p) {
        return 1;
    }
    else {
        return -1;
    }
}

char *strncpy(char *destination, char *source, int num) {
    char *ret = destination;

    while (*source && num-- > 0) {
        *destination++ = *source++;
    }

    while (num-- > 0) {
        *destination++ = '\0';
    }

    return ret;
}

// Find if a character is in a list
int chrnlst(char c, char *l) {
    for (int i = 0; l[i] != '\0'; i++) {
        if (c == l[i])
            return 1;
    }
    return 0;
}
