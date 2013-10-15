/*
 * Copyright (C) 2013 F4OS Authors
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
