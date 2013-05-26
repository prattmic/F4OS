#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#include <kernel/fault.h>

/* Set size bytes to value from p */
void memset32(void *p, int32_t value, uint32_t size) {
    uint32_t *end = (uint32_t *) ((uint32_t) p + size);

    /* Disallowed unaligned addresses */
    if ( (uint32_t) p % 4 ) {
        panic_print("Attempt to memset unaligned address (0x%x).", p);
    }

    while ( (uint32_t*) p < end ) {
        *((uint32_t*)p) = value;
        p++;
    }
}

/* Set size bytes to value from p */
void memset(void *p, uint8_t value, uint32_t size) {
    uint8_t *end = (uint8_t *) ((uint32_t) p + size);

    while ( (uint8_t*) p < end ) {
        *((uint8_t*)p) = value;
        p++;
    }
}

void memcpy(void *dst, void *src, int n) {
    while(n--) {
        *(uint8_t *)dst++ = *(uint8_t *)src++;
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

void itoa(int n, char buf[]) {
    int i, sign;

    if ((sign = n) < 0) {
        n = -n;
    }

    i = 0;
    do {
        buf[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0) {
        buf[i++] = '-';
    }

    buf[i] = '\0';

    strreverse(buf);
}

int atoi(char buf[]) {
    int result = 0;
    int negate = 0;
    int i = 0;
    // ignore trailing whitespace
    while (chrnlst(buf[i]," \t\r\n")) {
        i++;
    }
    // check for negative
    if (buf[i] == '-') {
        negate = 1;
        i++;
    }
    // parse decimal number
    while (buf[i] >= '0' && buf[i] <= '9') {
        result *= 10;
        result += (buf[i] - '0');
        i++;
    }
    return result * (negate ? -1 : 1);
}

void uitoa(uint32_t n, char buf[]) {
    int i = 0;

    do {
        buf[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    buf[i] = '\0';

    strreverse(buf);
}

void ftoa(float num, float tolerance, char buf[], uint32_t n) {
    int m;
    int digit;

    if (isnan(num)) {
        buf[0%n] = 'n';
        buf[1%n] = 'a';
        buf[2%n] = 'n';
        buf[3%n] = '\0';
        return;
    }
    if (isinf(num)) {
        if (ispos(num)) {
            buf[0%n] = 'i';
            buf[1%n] = 'n';
            buf[2%n] = 'f';
            buf[3%n] = '\0';
            return;
        }
        else {
            buf[0%n] = '-';
            buf[1%n] = 'i';
            buf[2%n] = 'n';
            buf[3%n] = 'f';
            buf[4%n] = '\0';
            return;
        }
    }

    if (num < 0.0f) {
        *(buf++) = '-';
        n--;
        num = -num;
    }

    m = logarithm(num, 1);

    if (m < 0) {
        *(buf++) = '0';
        *(buf++) = '.';
    }

    while ((num > 0 + tolerance || m >= 0) && n > 1) {
        float weight = powf(10.0f, m);
        digit = floorf(num / weight);
        num -= (digit*weight);
        *(buf++)= '0' + digit;
        n--;
        if (m == 0 && n > 1) {
            *(buf++) = '.';
            n--;
        }
        m--;
    }

    /* Print zero after decimal at end */
    if (*(buf-1) == '.' && n > 1) {
        *(buf++) = '0';
        n--;
    }

    *(buf) = '\0';
}

void strreverse(char *s) {
    char *begin = s;

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

    if (*s == *p) {
        return 0;
    }
    else if (*s > *p) {
        return 1;
    }
    else {
        return -1;
    }
}

// Find if a character is in a list
int chrnlst(char c, char *l) {
    for (int i = 0; l[i] != '\0'; i++) {
        if (c == l[i])
            return 1;
    }
    return 0;
}
