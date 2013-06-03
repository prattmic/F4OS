/* Portions of this file were based on source
 * code from Stack Overflow, and fall under the
 * Creative Commons Attribution-ShareAlike 2.5 Generic
 * license.  http://creativecommons.org/licenses/by-sa/2.5/ */

#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

char *strndup(char *str, int n) {
    int len = strnlen(str, n);

    char *dup = malloc(len+1);  // Room for NULL
    if (!dup) {
        return NULL;
    }

    strncpy(dup, str, len);
    dup[len] = '\0';

    return dup;
}

int atoi(char buf[]) {
    int result = 0;
    int negate = 0;
    int i = 0;

    // ignore leading whitespace
    while (chrnlst(buf[i]," \t\r\n")) {
        i++;
    }

    // check for negative
    if (buf[i] == '-') {
        negate = 1;
        i++;
    }
    else if (buf[i] == '+') {
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

/* Safely takes absolute value of an int,
 * properly handling INT_MIN */
static uint32_t int_abs(int n) {
    if (n > 0) {
        return n;
    }
    else {
        return UINT_MAX - (uint32_t) n + 1;
    }
}

void itoa(int n, char buf[]) {
    int sign = n;
    uint32_t num = int_abs(n);

    int i = 0;
    do {
        buf[i++] = num % 10 + '0';
    } while ((num /= 10) > 0);

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

/* Based on http://stackoverflow.com/a/2303798/10817 by Sophy Pal */
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
