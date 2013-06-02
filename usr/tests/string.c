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
            sprintf(message, "memory[%d] = %u", i, memory[i]);
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
            sprintf(message, "memory_dst[%d] = %u", i, memory_dst[i]);
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
            sprintf(message, "memory[%d] = %u", i, memory[i]);
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
            sprintf(message, "memory[%d] = %u", MEM_SIZE + i, memory[MEM_SIZE + i]);
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
            sprintf(message, "memory[%d] = %u", 3+i, memory[3+i]);
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
            sprintf(message, "memory[%d] = %u", i, memory[i]);
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

    for (int i = 0; i < sizeof(strings)/sizeof(strings[0]); i++) {
        int computed_len = strlen(strings[i].str);

        if (computed_len != strings[i].len) {
            sprintf(message, "strlen(\"%s\") != %d", strings[i].str, computed_len);
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

    for (int i = 0; i < sizeof(strings)/sizeof(strings[0]); i++) {
        int computed_len = strnlen(strings[i].str, 4);

        if (computed_len != strings[i].len) {
            sprintf(message, "strnlen(\"%s\", 4) != %d", strings[i].str, computed_len);
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

    for (int i = 0; i < sizeof(strings)/sizeof(strings[0]); i++) {
        char *str = strndup(strings[i].str, 25);
        if (!str) {
            strncpy(message, "strndup failed", len);
            return FAILED;
        }

        strreverse(str);

        if (strncmp(str, strings[i].rev, 25)) {
            free(str);
            sprintf(message, "%s != %s", str, strings[i].rev);
            return FAILED;
        }

        free(str);
    }

    return PASSED;
}
DEFINE_TEST("strreverse", strreverse_test);
