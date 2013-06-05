#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <dev/hw/perfcounter.h>
#include <mm/mm.h>
#include <math.h>
#include <list.h>
#include <kernel/sched.h>
#include <kernel/semaphore.h>
#include <dev/shared_deq.h>
#include "app.h"

/* Various configuration options for tests */
#define ITERATIONS      10
#define NUM_TEST_TASKS  20

struct
{
    int *a;
    uint8_t len;
    int idx;
} range = {
    .a = (int[]){8, 355, 22, 100, 1020, 5000, 450},
    .len = 7,
    .idx = 0,
};

typedef struct signal {
    LIST_ELEMENT;
} signal_t;

typedef struct pointerq {
    LIST_ELEMENT;
    void *p;
} pointerq_t;

DEFINE_SHARED_DEQ(pdeque);
DEFINE_SHARED_DEQ(killdeque);
PROF_DEFINE_COUNTER(mem_perf);

/* Helper for pop/free of one stored memory object */
static inline void free_one_pointerq(void) {
    list_t *l = __sdeq_pop(&pdeque);
    if(l) {
        pointerq_t *p = container_of(l, pointerq_t, _list);
        if(p->p)
            free(p->p);
        free(p);
    }
}

void allocator_worker(void) {
    list_t *l = __sdeq_pop(&killdeque);
    if(l) {
        free_one_pointerq();
        signal_t *sig = container_of(l, signal_t, _list);
        free(sig);
        abort();
    }
    else {
        free_one_pointerq();
        void* stuff = malloc(range.a[range.idx++]); /* Increment not atomic, but who cares */

        /* Not atomic, but who cares */
        if(range.idx >= 7)
            range.idx = 0;

        pointerq_t *p = malloc(sizeof(*p));
        if(p) {
            /* Don't care if stuff is NULL */
            p->p = stuff;
            sdeq_add(&pdeque, p);
        }
        else {
            printf("Worker %x failed to get memory for queueing pointer\r\n", curr_task);
        }
    }
}

void mem_perf(int argc, char **argv) {
    uint32_t times[ITERATIONS];
    uint32_t total;

    printf("ALLOCATOR BENCHMARKS\r\n");

    for(int n = 3; n < 16; n++) {
        total = 0;
        uint32_t blocksize = pow(2, n)-4;
        printf("Basic alloc %d bytes\r\n", blocksize);
        for(int i = 0; i < ITERATIONS; i++) {
            void *stuff = malloc(blocksize);
            if (!stuff) {
                printf("Warning: could not allocate %d bytes of memory\r\n", blocksize);
            }
            else {
                free(stuff);
            }

            times[i] = (uint32_t)PROF_COUNTER_DELTA(malloc);
            total += times[i];
            #ifdef VERBOSE
            printf("--Time Delta %d = %u\r\n", i, times[i]);
            #endif
        }
        printf("--Average time %fus\r\n", (total/((float)ITERATIONS))/(CONFIG_SYS_CLOCK/1e6));
    }
    
    
    printf("Task intensive benchmark\r\n");

    uint64_t start_test_malloc_total, end_test_malloc_total;
    uint64_t start_test_free_total, end_test_free_total;

    start_test_malloc_total = malloc_total;
    start_test_free_total = free_total;
    PROF_START_COUNTER(mem_perf);

    for(int i = 0; i < NUM_TEST_TASKS; i++) {
        new_task(&allocator_worker, 1, 11);
        usleep(500);
    }

    /* Wait for heap to get thorough workout */
    usleep(1000000);

    PROF_START_COUNTER(mem_perf);
    end_test_malloc_total = malloc_total;
    end_test_free_total = free_total;

    for(int i = 0; i < NUM_TEST_TASKS; i++) {
        signal_t *sig = malloc(sizeof(*sig));
        sdeq_add(&killdeque, sig);
    }

    printf("--Total time mallocing: %dus\r\n", ((uint32_t)(end_test_malloc_total - start_test_malloc_total))/168);
    printf("--Total time freeing: %dus\r\n", ((uint32_t)(end_test_free_total - start_test_free_total))/168);
    printf("--Total time spent: %dus\r\n", ((uint32_t)PROF_COUNTER_DELTA(mem_perf)/168));
}
DEFINE_APP(mem_perf)
