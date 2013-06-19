#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "test.h"

/* Tests for the string library functions */

#define MEM_SIZE    16
#define MEM_MAGIC   0xBE

int memset_test(char *message, int len) {
    uint8_t memory[MEM_SIZE] = {0};
    memset(memory, MEM_MAGIC, MEM_SIZE);

    for (int i = 0; i < MEM_SIZE; i++) {
        if (memory[i] != MEM_MAGIC) {
            scnprintf(message, len, "memory[%d] = %u", i, memory[i]);
            return FAILED;
        }
    }

    return PASSED;
}
DEFINE_TEST("memset", memset_test);

int memcpy_test(char *message, int len) {
    uint8_t memory_src[MEM_SIZE];
    uint8_t memory_dst[MEM_SIZE] = {0};

    memset(memory_src, MEM_MAGIC, MEM_SIZE);

    memcpy(memory_dst, memory_src, MEM_SIZE);

    for (int i = 0; i < MEM_SIZE; i++) {
        if (memory_dst[i] != MEM_MAGIC) {
            scnprintf(message, len, "memory_dst[%d] = %u", i, memory_dst[i]);
            return FAILED;
        }
    }

    return PASSED;
}
DEFINE_TEST("memcpy", memcpy_test);

int memmove_src_first_no_overlap(char *message, int len) {
    uint8_t memory[2*MEM_SIZE];

    /* Fill the first half with magic */
    memset(memory, MEM_MAGIC, MEM_SIZE);

    /* Copy magic to second half */
    memcpy(&memory[MEM_SIZE], memory, MEM_SIZE);

    for (int i = 0; i < MEM_SIZE; i++) {
        if (memory[i] != MEM_MAGIC) {
            scnprintf(message, len, "memory[%d] = %u", i, memory[i]);
            return FAILED;
        }
    }

    return PASSED;
}
DEFINE_TEST("memmove without overlap, source first", memmove_src_first_no_overlap);

int memmove_src_last_no_overlap(char *message, int len) {
    uint8_t memory[2*MEM_SIZE];

    /* Fill the second half with magic */
    memset(&memory[MEM_SIZE], MEM_MAGIC, MEM_SIZE);

    /* Copy magic to first half */
    memcpy(memory, &memory[MEM_SIZE], MEM_SIZE);

    for (int i = 0; i < MEM_SIZE; i++) {
        if (memory[MEM_SIZE + i] != MEM_MAGIC) {
            scnprintf(message, len, "memory[%d] = %u", MEM_SIZE + i, memory[MEM_SIZE + i]);
            return FAILED;
        }
    }

    return PASSED;
}
DEFINE_TEST("memmove without overlap, source last", memmove_src_last_no_overlap);

int memmove_src_first_overlap(char *message, int len) {
    /* [0, 1, 2, 3, 4, 5, 6, 7, 8, 0, 0, 0] ->
     * [0, 1, 2, 0, 1, 2, 3, 4, 5, 6, 7, 8] */
    uint8_t memory[12] = {0};

    for (int i = 0; i < 8; i++) {
        memory[i] = i;
    }

    memmove(&memory[3], memory, 8);

    for (int i = 0; i < 8; i++) {
        if (memory[3+i] != i) {
            scnprintf(message, len, "memory[%d] = %u", 3+i, memory[3+i]);
            return FAILED;
        }
    }

    return PASSED;
}
DEFINE_TEST("memmove with overlap, source first", memmove_src_first_overlap);

int memmove_src_last_overlap(char *message, int len) {
    /* [0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8] ->
     * [0, 1, 2, 3, 4, 5, 6, 7, 8, 6, 7, 8] */
    uint8_t memory[12] = {0};

    for (int i = 0; i < 8; i++) {
        memory[3+i] = i;
    }

    memmove(memory, &memory[3], 8);

    for (int i = 0; i < 8; i++) {
        if (memory[i] != i) {
            scnprintf(message, len, "memory[%d] = %u", i, memory[i]);
            return FAILED;
        }
    }

    return PASSED;
}
DEFINE_TEST("memmove with overlap, source last", memmove_src_last_overlap);

int strlen_test(char *message, int len) {
    struct {
        char *str;
        int len;
    } strings[] = {
        { .str = "", .len = 0 },
        { .str = "four", .len = 4 },
        { .str = "\r\t\n\b", .len = 4 },
        { .str = "thisisareallylongstring", .len = 23 },
    };

    for (int i = 0; i < ARRAY_LENGTH(strings); i++) {
        int computed_len = strlen(strings[i].str);

        if (computed_len != strings[i].len) {
            scnprintf(message, len, "strlen(\"%s\") != %d", strings[i].str, computed_len);
            return FAILED;
        }
    }

    return PASSED;
}
DEFINE_TEST("strlen", strlen_test);

int strnlen_test(char *message, int len) {
    struct {
        char *str;
        int len;
    } strings[] = {
        { .str = "", .len = 0 },
        { .str = "four", .len = 4 },
        { .str = "\r\t\n\b", .len = 4 },
        { .str = "thisisareallylongstring", .len = 4 }, // strnlen will stop counting
    };

    for (int i = 0; i < ARRAY_LENGTH(strings); i++) {
        int computed_len = strnlen(strings[i].str, 4);

        if (computed_len != strings[i].len) {
            scnprintf(message, len, "strnlen(\"%s\", 4) != %d", strings[i].str, computed_len);
            return FAILED;
        }
    }

    return PASSED;
}
DEFINE_TEST("strnlen", strnlen_test);

int strreverse_test(char *message, int len) {
    struct {
        char *str;
        char *rev;
    } strings[] = {
        { .str = "", .rev = "" },
        { .str = "four", .rev = "ruof" },
        { .str = "\r\t\n\b", .rev = "\b\n\t\r" },
    };

    for (int i = 0; i < ARRAY_LENGTH(strings); i++) {
        char *str = strndup(strings[i].str, 25);
        if (!str) {
            strncpy(message, "strndup failed", len);
            return FAILED;
        }

        strreverse(str);

        if (strncmp(str, strings[i].rev, 25)) {
            free(str);
            scnprintf(message, len, "%s != %s", str, strings[i].rev);
            return FAILED;
        }

        free(str);
    }

    return PASSED;
}
DEFINE_TEST("strreverse", strreverse_test);

int strncmp_test(char *message, int len) {
    struct {
        char *str1;
        char *str2;
        int ret;
    } cases[] = {
        { .str1 = "test", .str2 = "test", .ret = 0 },
        { .str1 = "test1", .str2 = "test2", .ret = -1 },
        { .str1 = "test2", .str2 = "test1", .ret = 1 },
        { .str1 = "012345678a", .str2 = "012345678b", .ret = -1 },
        { .str1 = "01234567890a", .str2 = "01234567890b", .ret = 0 },
        { .str1 = "0123456789a", .str2 = "0123456789b", .ret = 0 },
    };

    for (int i = 0; i < ARRAY_LENGTH(cases); i++) {
        int ret = strncmp(cases[i].str1, cases[i].str2, 10);

        if (ret != cases[i].ret) {
            scnprintf(message, len, "strncmp(\"%s\", \"%s\", 10) returned %d, not %d",
                    cases[i].str1, cases[i].str2, ret, cases[i].ret);
            return FAILED;
        }
    }

    return PASSED;
}
DEFINE_TEST("strncmp", strncmp_test);

int strncpy_test(char *message, int len) {
    char *str_src = "Hello World";
    char str_dst[25];

    memset(str_dst, MEM_MAGIC, 25);

    strncpy(str_dst, str_src, 25);

    if (strncmp(str_src, str_dst, 25) != 0) {
        scnprintf(message, len, "'%s' != '%s'", str_src, str_dst);
        return FAILED;
    }
    else if (str_dst[20] != '\0') {
        scnprintf(message, len, "Destination not NULLed");
        return FAILED;
    }

    return PASSED;
}
DEFINE_TEST("strncpy", strncpy_test);

int chrnlst_test(char *message, int len) {
    struct {
        char c;
        char *list;
        int ret;
    } cases[] = {
        { .c = '\0', .list = "", .ret = 0 },
        { .c = '\0', .list = " \t\r\n", .ret = 0 },
        { .c = '\t', .list = " \t\r\n", .ret = 1 },
        { .c = 'a', .list = "ABC", .ret = 0 },
        { .c = 'a', .list = "abc", .ret = 1 },
        { .c = 'a', .list = "c\r\na", .ret = 1 },
    };

    for (int i = 0; i < ARRAY_LENGTH(cases); i++) {
        int ret = chrnlst(cases[i].c, cases[i].list);

        if (ret != cases[i].ret) {
            scnprintf(message, len, "chrnlst(\"%c\", \"%s\") returned %d, not %d",
                    cases[i].c, cases[i].list, ret, cases[i].ret);
            return FAILED;
        }
    }

    return PASSED;
}
DEFINE_TEST("chrnlst", chrnlst_test);
