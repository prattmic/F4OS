#include "types.h"
#include "math.h"
#include "string.h"

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

    while ((num > 0 + tolerance || m >= 0) && n > 0) {
        float weight = powf(10.0f, m);
        digit = floorf(num / weight);
        num -= (digit*weight);
        *(buf++)= '0' + digit;
        n--;
        if (m == 0 && n > 0) {
            *(buf++) = '.';
            n--;
        }
        m--;
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
