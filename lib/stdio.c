#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <dev/resource.h>
#include <dev/buf_stream.h>

#include <stdio.h>

int fputs(rd_t rd, char *s) {
    return swrite(rd, s);
}

int puts(char *s) {
    return swrite(0, s);
}

int fputc(rd_t rd, char letter) {
    return write(rd, &letter, 1);
}

int putc(char letter) {
    return write(0, &letter, 1);
}

int fgetc(rd_t rd) {
    char ret;
    if (read(rd, &ret, 1) == 1) {
        return ret;
    }
    else {
        return -1;
    }
}

int sprintf(char *buf, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    rd_t stream = open_buf_stream(buf);
    int ret = vfprintf(stream, fmt, ap, &fputs, &fputc);
    close(stream);

    va_end(ap);

    return ret;
}

#define PS 256

void printx(char *s, uint8_t *x, int n) {
    char buf[PS];

    for (int i = 0; i < PS; i++){
        if(*s == '%'){
            for(int j = n-1; j >= 0; j--){
                buf[i++%128] = ((*(x+j)>>4)&0xf)[
                    "0123456789ABCDEF"
                ];
                buf[i++%128] = (*(x+j)&0xf)[
                    "0123456789ABCDEF"
                ];
            }
            i--;
        }
        else{
            buf[i] = *s;
        }
        s++;
    }
    puts(buf);
}

int fprintf(rd_t rd, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = vfprintf(rd, fmt, ap, &fputs, &fputc);
    va_end(ap);

    return ret;
}

static inline int holding_flush(char *holding, int *hold_count, rd_t rd, int (*puts_fn)(rd_t,char*)) {
    if (*hold_count) {
        int ret = puts_fn(rd, holding);
        if (ret >= 0) {
            *hold_count = 0;
            holding[0] = '\0';
        }
        return ret;
    }
    else {
        return 0;
    }
}

static inline int holding_push(char c, char *holding, int hold_len, int *hold_count, rd_t rd, int (*puts_fn)(rd_t,char*)) {
    int ret;

    if (*hold_count >= hold_len - 1) {
        ret = holding_flush(holding, hold_count, rd, puts_fn);
    }
    else {
        ret = 0;
    }

    holding[(*hold_count)++] = c;
    holding[*hold_count] = '\0';

    return ret;
}

/* Returns bytes written, negative on error */
int vfprintf(rd_t rd, char *fmt, va_list ap, int (*puts_fn)(rd_t,char*), int (*putc_fn)(rd_t,char)) {
    int total = 0;

    /* We buffer data here as long as possible, because the actual puts/putc functions tend to be slow
     * and the fewer calls we can make, the better. */
    char holding[32];
    int hold_count = 0;
    const int hold_len = sizeof(holding)/sizeof(holding[0]);

    holding[0] = '\0';

    int ret;

    while (*fmt) {
        if (*fmt == '%') {
            switch (*(++fmt)) {
                case 'x': {
                    /* Hex */
                    uint32_t hex = va_arg(ap, uint32_t);
                    char buf[9];
                    buf[8] = '\0';

                    uint8_t i = 0;
                    for(int8_t j = 7; j >= 0; j--){
                        buf[i++] = ((hex>>(4*j))&0xf)[
                            "0123456789ABCDEF"
                        ];
                    }

                    ret = holding_flush(holding, &hold_count, rd, puts_fn);
                    if (ret >= 0) {
                        total += ret;
                    }
                    else {
                        return ret;
                    }

                    ret = puts_fn(rd, buf);
                    if (ret >= 0) {
                        total += ret;
                    }
                    else {
                        return ret;
                    }

                    break;
                }
                case 'i': case 'd': {
                    int num = va_arg(ap, int);
                    char buf[9];    /* 7 digits in INT_MAX + '-' and '\0' */

                    itoa(num, buf);

                    ret = holding_flush(holding, &hold_count, rd, puts_fn);
                    if (ret >= 0) {
                        total += ret;
                    }
                    else {
                        return ret;
                    }

                    ret = puts_fn(rd, buf);
                    if (ret >= 0) {
                        total += ret;
                    }
                    else {
                        return ret;
                    }
                    break;
                }
                case 'u': {
                    uint32_t num = va_arg(ap, uint32_t);
                    char buf[9];    /* 7 digits in INT_MAX + '-' and '\0' */

                    uitoa(num, buf);

                    ret = holding_flush(holding, &hold_count, rd, puts_fn);
                    if (ret >= 0) {
                        total += ret;
                    }
                    else {
                        return ret;
                    }

                    ret = puts_fn(rd, buf);
                    if (ret >= 0) {
                        total += ret;
                    }
                    else {
                        return ret;
                    }
                    break;
                }
                case 'f': {
                    float num = (float) va_arg(ap, double);
                    char buf[20];

                    ftoa(num, 0.0001f, buf, 20);

                    ret = holding_flush(holding, &hold_count, rd, puts_fn);
                    if (ret >= 0) {
                        total += ret;
                    }
                    else {
                        return ret;
                    }

                    ret = puts_fn(rd, buf);
                    if (ret >= 0) {
                        total += ret;
                    }
                    else {
                        return ret;
                    }
                    break;
                }
                case 'c': {
                    char letter = (char) va_arg(ap, uint32_t);

                    ret = holding_push(letter, holding, hold_len, &hold_count, rd, puts_fn);
                    if (ret >= 0) {
                        total += ret;
                    }
                    else {
                        return ret;
                    }

                    break;
                }
                case 's': {
                    char *s = va_arg(ap, char*);

                    ret = holding_flush(holding, &hold_count, rd, puts_fn);
                    if (ret >= 0) {
                        total += ret;
                    }
                    else {
                        return ret;
                    }

                    ret = puts_fn(rd, s);
                    if (ret >= 0) {
                        total += ret;
                    }
                    else {
                        return ret;
                    }

                    break;
                }
                case '%': { /* Just print a % */
                    ret = holding_push('%', holding, hold_len, &hold_count, rd, puts_fn);
                    if (ret >= 0) {
                        total += ret;
                    }
                    else {
                        return ret;
                    }

                    break;
                }
                default: {
                    ret = holding_push('%', holding, hold_len, &hold_count, rd, puts_fn);
                    if (ret >= 0) {
                        total += ret;
                    }
                    else {
                        return ret;
                    }

                    ret = holding_push(*fmt, holding, hold_len, &hold_count, rd, puts_fn);
                    if (ret >= 0) {
                        total += ret;
                    }
                    else {
                        return ret;
                    }
                }
            }

            fmt++;
        }
        else {
            ret = holding_push(*fmt++, holding, hold_len, &hold_count, rd, puts_fn);
            if (ret >= 0) {
                total += ret;
            }
            else {
                return ret;
            }
        }
    }

    ret = holding_flush(holding, &hold_count, rd, puts_fn);
    if (ret >= 0) {
        total += ret;
    }
    else {
        return ret;
    }

    return total;
}
