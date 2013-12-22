/*
 * Copyright (C) 2013, 2014 F4OS Authors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "test.h"

/* Tests for the string library functions */

int memchr_test(char *message, int len) {
    struct {
        char *mem;
        int val;
        size_t num;
        char *addr;
    } strings[] = {
        [0] = { .mem = "", .val = 'a', .num = 1, .addr = NULL },
        [1] = { .mem = "four", .val = 'u', .num = 4,
                .addr = &strings[1].mem[2] },
        [2] = { .mem = "\r\t\n\b", .val = 'a', .num = 4, .addr = NULL },
        [3] = { .mem = "this\0is\0a", .val = 'a', .num = 9,
                .addr = &strings[3].mem[8] },
    };

    for (int i = 0; i < ARRAY_LENGTH(strings); i++) {
        char *ret = memchr(strings[i].mem, strings[i].val, strings[i].num);

        if (ret != strings[i].addr) {
            scnprintf(message, len, "memchr(\"%s\", '%c', %d) != %d",
                      strings[i].mem, strings[i].val, strings[i].num,
                      strings[i].addr);
            return FAILED;
        }
    }

    return PASSED;
}
DEFINE_TEST("memchr", memchr_test);

int memcmp_test(char *message, int len) {
    struct {
        char *mem1;
        char *mem2;
        size_t num;
        int ret;
    } strings[] = {
        { .mem1 = "hello", .mem2 = "hello", .num = 5, .ret = 0 },
        { .mem1 = "hello1", .mem2 = "hello", .num = 5, .ret = 0 },
        { .mem1 = "hello1", .mem2 = "hello2", .num = 6, .ret = -1 },
        { .mem1 = "hello2", .mem2 = "hello1", .num = 6, .ret = 1 },
    };

    for (int i = 0; i < ARRAY_LENGTH(strings); i++) {
        int ret = memcmp(strings[i].mem1, strings[i].mem2, strings[i].num);

        if (ret != strings[i].ret) {
            scnprintf(message, len, "memcmp(\"%s\", \"%s\", %d) != %d",
                      strings[i].mem1, strings[i].mem2, strings[i].num,
                      strings[i].ret);
            return FAILED;
        }
    }

    return PASSED;
}
DEFINE_TEST("memcmp", memcmp_test);

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

int strchr_test(char *message, int len) {
    struct {
        char *mem;
        int val;
        char *addr;
    } strings[] = {
        [0] = { .mem = "", .val = 'a', .addr = NULL },
        [1] = { .mem = "four", .val = 'u', .addr = &strings[1].mem[2] },
        [2] = { .mem = "four", .val = '\0', .addr = &strings[2].mem[4] },
        [3] = { .mem = "\r\t\n\b", .val = 'a', .addr = NULL },
        [4] = { .mem = "this\0is\0a", .val = 'a', .addr = NULL },
    };

    for (int i = 0; i < ARRAY_LENGTH(strings); i++) {
        char *ret = strchr(strings[i].mem, strings[i].val);

        if (ret != strings[i].addr) {
            scnprintf(message, len, "strchr(\"%s\", '%c') != %d",
                      strings[i].mem, strings[i].val, strings[i].addr);
            return FAILED;
        }
    }

    return PASSED;
}
DEFINE_TEST("strchr", strchr_test);

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

int strcmp_test(char *message, int len) {
    struct {
        char *str1;
        char *str2;
        int ret;
    } cases[] = {
        { .str1 = "test", .str2 = "test", .ret = 0 },
        { .str1 = "test1", .str2 = "test2", .ret = -1 },
        { .str1 = "test2", .str2 = "test1", .ret = 1 },
        { .str1 = "012345678a", .str2 = "012345678b", .ret = -1 },
        { .str1 = "01234567890", .str2 = "01234567890", .ret = 0 },
        { .str1 = "0123456789", .str2 = "0123456789", .ret = 0 },
    };

    for (int i = 0; i < ARRAY_LENGTH(cases); i++) {
        int ret = strcmp(cases[i].str1, cases[i].str2);

        if (ret != cases[i].ret) {
            scnprintf(message, len, "strcmp(\"%s\", \"%s\") returned %d, not %d",
                    cases[i].str1, cases[i].str2, ret, cases[i].ret);
            return FAILED;
        }
    }

    return PASSED;
}
DEFINE_TEST("strcmp", strcmp_test);

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
