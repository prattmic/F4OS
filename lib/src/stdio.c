#include "types.h"
#include "task.h"
#include "mem.h"
#include "resource.h"
#include "string.h"
#include "stdarg.h"
#include "stdio.h"

void puts(char *s) {
    write(0, s);
}

char getc(void) {
    return read(0);
}

void putc(char letter) {
    char string[2];
    string[0] = letter;
    string[1] = '\0';

    puts(string);
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

void printf(char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);

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

                    puts(buf);
                    break;
                }
                case 'i': case 'd': {
                    int num = va_arg(ap, int);
                    char buf[9];    /* 7 digits in INT_MAX + '-' and '\0' */

                    itoa(num, buf);

                    puts(buf);
                    break;
                }
                case 'u': {
                    uint32_t num = va_arg(ap, uint32_t);
                    char buf[9];    /* 7 digits in INT_MAX + '-' and '\0' */

                    uitoa(num, buf);

                    puts(buf);
                    break;
                }
                case 'f': {
                    float num = va_arg(ap, float);
                    char buf[20];

                    ftoa(num, 0.0001f, buf, 20);

                    puts(buf);
                    break;
                }
                case 'c': {
                    char letter = va_arg(ap, char);

                    putc(letter);
                    break;
                }
                case 's': {
                    char *s = va_arg(ap, char*);

                    puts(s);
                    break;
                }
                case '%': {
                    /* Just print a % */
                    putc('%');
                    break;
                }
                default: {
                    putc('%');
                    putc(*fmt);
                }
            }

            fmt++;
        }
        else {
            putc(*fmt++);
        }
    }

    va_end(ap);
}
