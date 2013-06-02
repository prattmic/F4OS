#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
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
