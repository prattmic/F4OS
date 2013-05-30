#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <kernel/sched.h>
#include <dev/registers.h>
#include <kernel/semaphore.h>
#include <list.h>
#include <dev/shared_deq.h>

#include "app.h"

DEFINE_SHARED_DEQ(killdeque)

typedef struct signal {
    LIST_ELEMENT;
} signal_t;

static inline uint64_t getcount(void) {
    return (*TIM2_CNT << 16)|(*TIM1_CNT);
}

void printer(void) {
    static uint32_t times[10];
    static uint8_t i = 0;
    static uint64_t start, end;
    if(!sdeq_empty(&killdeque)) {
        signal_t *sig = sdeq_entry(sdeq_pop(&killdeque), signal_t);
        free(sig);
        abort();
    }
    else {
        end = getcount();
        if (i < 10) {
            times[i++] = (uint32_t)(end-start);
        }
        else {
            for(i = 0; i < 10; i++) {
                printf("Time Delta %d = %u\r\n", i, times[i]);
            }
            i = 0;
        }
        start = getcount();
    }
}

void mem_perf(int argc, char **argv) {
    printf("Press q to quit, any key to continue.\r\n");

    new_task(&printer, 1, CONFIG_SYSTICK_FREQ);

    while (getc() != 'q') {};

    signal_t *sig = malloc(sizeof(list_t));
    sdeq_add(&killdeque, sig);
}
DEFINE_APP(mem_perf)
