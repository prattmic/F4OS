#include <stdio.h>
#include <stdlib.h>
#include <list.h>
#include "app.h"

struct message {
    int num;
    struct list list;
};

void list_test(int argc, char **argv) {
    struct list list = INIT_LIST(list);

    printf("Filling list\r\n");
    for (int i = 0; i < 5; i++) {
        struct message *m = malloc(sizeof(struct message));
        if (!m) {
            printf("Malloc failed\r\n");
            return;
        }

        m->num = i;
        list_add(&m->list, &list);
        printf("Message = %d\r\n", m->num);
    }

    printf("Entries using list_for_each\r\n");
    struct list *curr;
    list_for_each(curr, &list) {
        struct message *m = container_of(curr, struct message, list);
        printf("Message = %d\r\n", m->num);
    }

    printf("Entries using list_for_each_entry\r\n");
    struct message *m;
    list_for_each_entry(m, &list, list) {
        printf("Message = %d\r\n", m->num);
    }

    printf("Popping elements\r\n");
    while (!list_empty(&list)) {
        curr = list_pop(&list);
        struct message *m = container_of(curr, struct message, list);
        printf("Message = %d\r\n", m->num);
        free(m);
    }
}
DEFINE_APP(list_test);
