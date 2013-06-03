#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <mm/mm.h>
#include <math.h>
#include "app.h"

#define ITERATIONS 10

void mem_perf(int argc, char **argv) {
    uint32_t times[ITERATIONS];
    uint32_t avg;

    avg = 0;
    printf("ALLOCATOR BENCHMARKS\r\n");

    for(int n = 3; n < 17; n++) {
        avg = 0;
        uint32_t blocksize = pow(2, n);
        printf("Basic alloc %d bytes\r\n", blocksize);
        for(int i = 0; i < ITERATIONS; i++) {
            void *stuff = malloc(blocksize);
            if (!stuff) {
                printf("Warning: no memory available\r\n");
            }
            else {
                free(stuff);
            }

            times[i] = (uint32_t)(end_malloc_timestamp - begin_malloc_timestamp);
            avg += times[i];
            #ifdef VERBOSE
            printf("--Time Delta %d = %u\r\n", i, times[i]);
            #endif
        }
        printf("--Average time %fus\r\n", (avg/((float)ITERATIONS))/(CONFIG_SYS_CLOCK/1e6));
    }
}
DEFINE_APP(mem_perf)
