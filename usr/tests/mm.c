#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <mm/mm.h>
#include "test.h"
#include <limits.h>

int malloc_simple(char *message, int len) {
    void *mem = malloc(1024);
    if (!mem) {
        return FAILED;
    }

    free(mem);
    return PASSED;
}
DEFINE_TEST("Simple malloc", malloc_simple);

int kmalloc_simple(char *message, int len) {
    void *mem = kmalloc(1024);
    if (!mem) {
        return FAILED;
    }

    kfree(mem);
    return PASSED;
}
DEFINE_TEST("Simple kmalloc", kmalloc_simple);

int malloc_range(char *message, int len) {
    uint32_t sizes[] = {1, 4, 8, 16, 32, 64, 128,
        256, 512, 1024, 2048, 4096, 8192, 16384,
        37, 353, 7894, 10000, 2560};

    for (int i = 0; i < ARRAY_LENGTH(sizes); i++) {
        void *mem = malloc(sizes[i]);
        if (!mem) {
            scnprintf(message, len, "Allocation of %d bytes failed", sizes[i]);
            return FAILED;
        }

        free(mem);
    }

    return PASSED;
}
DEFINE_TEST("malloc range", malloc_range);

int kmalloc_range(char *message, int len) {
    uint32_t sizes[] = {1, 4, 8, 16, 32, 64, 128,
        256, 512, 1024, 2048, 4096, 8192, 37, 353,
        7894, 1000, 2560};

    for (int i = 0; i < ARRAY_LENGTH(sizes); i++) {
        void *mem = kmalloc(sizes[i]);
        if (!mem) {
            scnprintf(message, len, "Allocation of %d bytes failed", sizes[i]);
            return FAILED;
        }

        kfree(mem);
    }

    return PASSED;
}
DEFINE_TEST("kmalloc range", kmalloc_range);

int malloc_toobig(char *message, int len) {
    void *mem = malloc(UINT_MAX);
    if(mem) {
        scnprintf(message, len, "Malloc of too much memory did not return NULL");
        free(mem);  /* wat */
        return FAILED;
    }

    return PASSED;
}
DEFINE_TEST("malloc too big", malloc_toobig);

int kmalloc_toobig(char *message, int len) {
    void *mem = kmalloc(UINT_MAX);
    if(mem) {
        scnprintf(message, len, "Malloc of too much memory did not return NULL");
        kfree(mem);  /* wat */
        return FAILED;
    }

    return PASSED;
}
DEFINE_TEST("kmalloc too big", kmalloc_toobig);
