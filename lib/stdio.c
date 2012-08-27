#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <dev/resource.h>

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

                    fputs(rd, buf);
                    break;
                }
                case 'i': case 'd': {
                    int num = va_arg(ap, int);
                    char buf[9];    /* 7 digits in INT_MAX + '-' and '\0' */

                    itoa(num, buf);

                    fputs(rd, buf);
                    break;
                }
                case 'u': {
                    uint32_t num = va_arg(ap, uint32_t);
                    char buf[9];    /* 7 digits in INT_MAX + '-' and '\0' */

                    uitoa(num, buf);

                    fputs(rd, buf);
                    break;
                }
                case 'f': {
                    float num = va_arg(ap, float);
                    char buf[20];

                    ftoa(num, 0.0001f, buf, 20);

                    fputs(rd, buf);
                    break;
                }
                case 'c': {
                    char letter = va_arg(ap, char);

                    fputc(rd, letter);
                    break;
                }
                case 's': {
                    char *s = va_arg(ap, char*);

                    fputs(rd, s);
                    break;
                }
                case '%': {
                    /* Just print a % */
                    fputc(rd, '%');
                    break;
                }
                default: {
                    fputc(rd, '%');
                    fputc(rd, *fmt);
                }
            }

            fmt++;
        }
        else {
            fputc(rd, *fmt++);
        }
    }

    va_end(ap);
}
