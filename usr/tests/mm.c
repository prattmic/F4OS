#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <mm/mm.h>
#include "test.h"

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
