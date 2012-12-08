#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <dev/resource.h>
#include <dev/buf_stream.h>

#include <stdio.h>

void fputs(rd_t rd, char *s) {
    swrite(rd, s);
}

void puts(char *s) {
    swrite(0, s);
}

void fputc(rd_t rd, char letter) {
    write(rd, &letter, 1);
}

void putc(char letter) {
    write(0, &letter, 1);
}

char fgetc(rd_t rd) {
    char ret;
    read(rd, &ret, 1);
    return ret;
}

char getc(void) {
    return fgetc(0);
}

void sprintf(char *buf, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    rd_t stream = open_buf_stream(buf);
    vfprintf(stream, fmt, ap, &fputs, &fputc);
    close(stream);

    va_end(ap);
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

void fprintf(rd_t rd, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(rd, fmt, ap, &fputs, &fputc);
    va_end(ap);
}

static inline void holding_flush(char *holding, int *hold_count, rd_t rd, void (*puts_fn)(rd_t,char*)) {
    if (*hold_count) {
        puts_fn(rd, holding);
        *hold_count = 0;
        holding[0] = '\0';
    }
}

static inline void holding_push(char c, char *holding, int hold_len, int *hold_count, rd_t rd, void (*puts_fn)(rd_t,char*)) {
    if (*hold_count >= hold_len - 1) {
        holding_flush(holding, hold_count, rd, puts_fn);
    }

    holding[(*hold_count)++] = c;
    holding[*hold_count] = '\0';
}

void vfprintf(rd_t rd, char *fmt, va_list ap, void (*puts_fn)(rd_t,char*), void (*putc_fn)(rd_t,char)) {
    /* We buffer data here as long as possible, because the actual puts/putc functions tend to be slow
     * and the fewer calls we can make, the better. */
    char holding[32];
    int hold_count = 0;
    const int hold_len = sizeof(holding)/sizeof(holding[0]);

    holding[0] = '\0';

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

                    holding_flush(holding, &hold_count, rd, puts_fn);

                    puts_fn(rd, buf);
                    break;
                }
                case 'i': case 'd': {
                    int num = va_arg(ap, int);
                    char buf[9];    /* 7 digits in INT_MAX + '-' and '\0' */

                    itoa(num, buf);

                    holding_flush(holding, &hold_count, rd, puts_fn);

                    puts_fn(rd, buf);
                    break;
                }
                case 'u': {
                    uint32_t num = va_arg(ap, uint32_t);
                    char buf[9];    /* 7 digits in INT_MAX + '-' and '\0' */

                    uitoa(num, buf);

                    holding_flush(holding, &hold_count, rd, puts_fn);

                    puts_fn(rd, buf);
                    break;
                }
                case 'f': {
                    float num = (float) va_arg(ap, double);
                    char buf[20];

                    ftoa(num, 0.0001f, buf, 20);

                    holding_flush(holding, &hold_count, rd, puts_fn);

                    puts_fn(rd, buf);
                    break;
                }
                case 'c': {
                    char letter = (char) va_arg(ap, uint32_t);

                    holding_push(letter, holding, hold_len, &hold_count, rd, puts_fn);
                    break;
                }
                case 's': {
                    char *s = va_arg(ap, char*);

                    holding_flush(holding, &hold_count, rd, puts_fn);

                    puts_fn(rd, s);
                    break;
                }
                case '%': { /* Just print a % */
                    holding_push('%', holding, hold_len, &hold_count, rd, puts_fn);
                    break;
                }
                default: {
                    holding_push('%', holding, hold_len, &hold_count, rd, puts_fn);
                    holding_push(*fmt, holding, hold_len, &hold_count, rd, puts_fn);
                }
            }

            fmt++;
        }
        else {
            holding_push(*fmt++, holding, hold_len, &hold_count, rd, puts_fn);
        }
    }

    holding_flush(holding, &hold_count, rd, puts_fn);
}
