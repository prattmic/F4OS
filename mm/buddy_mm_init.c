/*
 * Copyright (C) 2013, 2014 F4OS Authors
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

#include <stddef.h>
#include <stdint.h>

#include "buddy_mm_internals.h"

/*
 * The top level of the buddy heap is 2^(max_order) bytes.
 * It is a configuration error if this does not fit within the defined heap.
 */
_Static_assert((1 << CONFIG_MM_USER_MAX_ORDER) <=
               (CONFIG_EUSERHEAP - CONFIG_SUSERHEAP),
               "CONFIG_MM_USER_MAX_ORDER is too large to fit in user heap");
_Static_assert((1 << CONFIG_MM_KERNEL_MAX_ORDER) <=
               (CONFIG_EKERNELHEAP - CONFIG_SKERNELHEAP),
               "CONFIG_MM_KERNEL_MAX_ORDER is too large to fit in kernel heap");

struct buddy user_buddy;
/* Use one extra word so that we index with order directly, instead of order-1 */
struct heapnode *user_buddy_list[CONFIG_MM_USER_MAX_ORDER+1];

struct buddy kernel_buddy;
struct heapnode *kernel_buddy_list[CONFIG_MM_KERNEL_MAX_ORDER+1];

static void init_buddy(struct buddy *buddy, void *address) __attribute__((section(".kernel")));

void init_heap(void) {
    /* User buddy */
    user_buddy.max_order = CONFIG_MM_USER_MAX_ORDER;
    user_buddy.min_order = CONFIG_MM_USER_MIN_ORDER;
    init_mutex(&user_buddy.mutex);
    user_buddy.list = user_buddy_list;

    init_buddy(&user_buddy, (void *)CONFIG_SUSERHEAP);

    /* Kernel buddy */
    kernel_buddy.max_order = CONFIG_MM_KERNEL_MAX_ORDER;
    kernel_buddy.min_order = CONFIG_MM_KERNEL_MIN_ORDER;
    init_mutex(&kernel_buddy.mutex);
    kernel_buddy.list = kernel_buddy_list;

    init_buddy(&kernel_buddy, (void *)CONFIG_SKERNELHEAP);
}

static void init_buddy(struct buddy *buddy, void *address) {
    for (int i = 0; i < buddy->max_order; i++) {
        buddy->list[i] = NULL;
    }

    buddy->list[buddy->max_order] = (struct heapnode *) address;
    buddy->list[buddy->max_order]->header.magic = MM_MAGIC;
    buddy->list[buddy->max_order]->header.order = buddy->max_order;
    buddy->list[buddy->max_order]->next = NULL;
}
