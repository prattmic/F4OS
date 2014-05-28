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

#include <stdint.h>
#include <math.h>

#include <mm/mm.h>
#include "buddy_mm_internals.h"

static uint32_t free_memory(struct buddy *buddy);

uint32_t mm_space(void) {
    uint32_t space;
    acquire(&user_buddy.mutex);
    space = free_memory(&user_buddy);
    release(&user_buddy.mutex);

    return space;
}

uint32_t mm_kspace(void) {
    uint32_t space;
    acquire(&kernel_buddy.mutex);
    space = free_memory(&kernel_buddy);
    release(&kernel_buddy.mutex);

    return space;
}

static uint32_t free_memory(struct buddy *buddy) {
    uint32_t free = 0;

    for (int i = buddy->min_order; i <= buddy->max_order; i++) {
        struct heapnode *node = buddy->list[i];
        while (node) {
            free += pow(2, i);
            node = node->next;
        }
    }

    return free;
}
