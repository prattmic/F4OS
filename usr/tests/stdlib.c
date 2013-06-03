#include <stdio.h>
#include <stdlib.h>
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

    for (int i = 0; i < sizeof(cases)/sizeof(cases[0]); i++) {
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
