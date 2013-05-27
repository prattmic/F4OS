#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <kernel/sched.h>
#include <dev/resource.h>
#include <list.h>
#include <kernel/semaphore.h>
#include <dev/shared_deq.h>

#include "app.h"

DEFINE_SHARED_DEQ(deque)

typedef struct message {
    struct list list;
    char *m;
} message_t;

void t1(void) {
    if(!sdeq_empty(&deque)) {
        message_t *m = container_of(sdeq_pop(&deque), message_t, list);
        printf("T1 got: %s\r\n", m->m);
        free(m);
        message_t *mret = malloc(sizeof(message_t));
        if (mret == NULL) {
            printf("Unable to malloc T1 mret\r\n");
            return;
        }
        mret->m = "T1 here!";
        sdeq_add(&deque, &(mret->list));
    }
    else {
        printf("Goodbye from T1!\r\n");
        abort();
    }
}

void t2(void) {
    if(!sdeq_empty(&deque)) {
        message_t *m = container_of(sdeq_pop(&deque), message_t, list);
        printf("T2 got: %s\r\n", m->m);
        free(m);
        message_t *mret = malloc(sizeof(message_t));
        if (mret == NULL) {
            printf("Unable to malloc T2 mret\r\n");
            return;
        }
        mret->m = "T2 here!";
        sdeq_add(&deque, &(mret->list));
    }
    else {
        printf("Goodbye from T2!\r\n");
        abort();
    }
}

void deq_test(int argc, char **argv) {
    if (argc != 1) {
        printf("Usage: %s\r\n", argv[0]);
        return;
    }

    message_t *m1 = malloc(sizeof(message_t));
    if (m1 == NULL) {
        printf("Unable to malloc m1\r\n");
        return;
    }
    m1->m = "Message 1";

    message_t *m2 = malloc(sizeof(message_t));
    if (m1 == NULL) {
        printf("Unable to malloc m1\r\n");
        return;
    }
    m2->m = "Message 2";

    message_t *m3 = malloc(sizeof(message_t));
    if (m1 == NULL) {
        printf("Unable to malloc m1\r\n");
        return;
    }
    m3->m = "Message 3";

    sdeq_add(&deque, &(m1->list));
    sdeq_add(&deque, &(m2->list));
    sdeq_add(&deque, &(m3->list));

    new_task(&t1, 1, 40);
    new_task(&t2, 1, 100);
    yield_if_possible();

    /* Make sure other tasks are done */
    usleep(50000);

    if(sdeq_empty(&deque)) {
        printf("Nothing...\r\n");
    }

    message_t *curr;
    while(!sdeq_empty(&deque)) {
        curr = container_of(sdeq_pop(&deque), message_t, list);
        printf("deq_test got: %s\r\n", curr->m);
        free(curr);
    }
}
DEFINE_APP(deq_test)
