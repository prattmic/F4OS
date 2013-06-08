#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test.h"

int atoi_test(char *message, int len) {
    struct {
        char *str;
        int num;
    } cases[] = {
        { .str = "0", .num = 0 },
        { .str = "-0", .num = 0 },
        { .str = "-1", .num = -1 },
        { .str = "+1", .num = +1 },
        { .str = "123", .num = 123 },
        { .str = "   -123", .num = -123 },
        { .str = "abc", .num = 0 },
        { .str = "10abc", .num = 10 },
        { .str = "2147483647", .num = 2147483647 },
        { .str = "-2147483648", .num = -2147483648 },
    };

    for (int i = 0; i < ARRAY_LENGTH(cases); i++) {
        int result = atoi(cases[i].str);

        if (result != cases[i].num) {
            sprintf(message, "atoi(\"%s\") = %d, should be %d",
                    cases[i].str, result, cases[i].num);
            return FAILED;
        }
    }

    return PASSED;
}
DEFINE_TEST("atoi", atoi_test);

int uitoa_test(char *message, int len) {
    struct {
        int num;
        char *str;
        int base;
    } cases[] = {
        { .num = 0, .str = "0", .base = 10 },
        { .num = 1, .str = "1", .base = 10 },
        { .num = 2147483647, .str = "2147483647", .base = 10 },
        { .num = 0x0BADF00D, .str = "BADF00D", .base = 16 },
        { .num = 0b11111111111111111111111111111111,
            .str = "11111111111111111111111111111111", .base = 2 },
        { .num = 0704532, .str = "704532", .base = 8 },
        { .num = 0xDEADBEEF, .str = "DEADBEEF", .base = 16 },
        { .num = 100000, .str = "255S", .base = 36 },
    };

    for (int i = 0; i < ARRAY_LENGTH(cases); i++) {
        char buf[40] = {'\0'};
        uitoa(cases[i].num, buf, 40, cases[i].base);

        if (strncmp(cases[i].str, buf, 40)) {
            sprintf(message, "uitoa(%d, base %d) = \"%s\", should be \"%s\"",
                    cases[i].num, cases[i].base, buf, cases[i].str);
            return FAILED;
        }
    }

    return PASSED;
}
DEFINE_TEST("uitoa", uitoa_test);

int itoa_test(char *message, int len) {
    struct {
        int num;
        char *str;
        int base;
    } cases[] = {
        { .num = 0, .str = "0", .base = 10 },
        { .num = -0, .str = "0", .base = 10 },
        { .num = 1, .str = "1", .base = 10 },
        { .num = -1, .str = "-1", .base = 10},
        { .num = 2147483647, .str = "2147483647", .base = 10 },
        { .num = -2147483648, .str = "-2147483648", .base = 10 },
        { .num = 0x0BADF00D, .str = "BADF00D", .base = 16 },
        { .num = 0b1101, .str = "1101", .base = 2 },
        { .num = 02222, .str = "2222", .base = 8 },
    };

    for (int i = 0; i < ARRAY_LENGTH(cases); i++) {
        char buf[20] = {'\0'};
        itoa(cases[i].num, buf, 20, cases[i].base);

        if (strncmp(cases[i].str, buf, 20)) {
            sprintf(message, "itoa(%d, base %d) = \"%s\", should be \"%s\"",
                    cases[i].num, cases[i].base, buf, cases[i].str);
            return FAILED;
        }
    }

    return PASSED;
}
DEFINE_TEST("itoa", itoa_test);

int ftoa_test(char *message, int len) {
    struct {
        float num;
        char *str;
        float tolerance;
    } cases[] = {
        { .num = 0.0f, .str = "0.0", .tolerance = 0.01 },
        { .num = 0.1f, .str = "0.1", .tolerance = 0.01 },
        { .num = 0.1f, .str = "0.1", .tolerance = 0.0001 },
        { .num = -0.1f, .str = "-0.1", .tolerance = 0.0001 },
        { .num = 1e-25f, .str = "0.0", .tolerance = 0.00001 },
        /* ftoa needs a more formal definition of expected behavior */
        { .num = 45.24452f, .str = "45.24451", .tolerance = 0.00001 },
        { .num = uint_to_float(FLOAT_INF), .str = "inf", .tolerance = 0.00001 },
        { .num = -uint_to_float(FLOAT_INF), .str = "-inf", .tolerance = 0.00001 },
        { .num = uint_to_float(FLOAT_NAN), .str = "nan", .tolerance = 0.00001 },
    };

    for (int i = 0; i < ARRAY_LENGTH(cases); i++) {
        char buf[20] = {'\0'};
        ftoa(cases[i].num, cases[i].tolerance, buf, 20);

        if (strncmp(cases[i].str, buf, 20)) {
            sprintf(message, "ftoa(%f) = \"%s\", should be \"%s\"",
                    cases[i].num, buf, cases[i].str);
            return FAILED;
        }
    }

    return PASSED;
}
DEFINE_TEST("ftoa", ftoa_test);
