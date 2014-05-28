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

#ifndef MM_BUDDY_MM_INTERNALS_INCLUDED
#define MM_BUDDY_MM_INTERNALS_INCLUDED

#include <kernel/mutex.h>

#define MM_MAGIC    0xBEEF

struct heapnode_header {
    uint16_t magic;
    uint8_t order;
    uint8_t padding;    /* Tasks won't take kindly to */
                        /* getting unaligned addresses */
} __attribute__((packed));

struct heapnode {
    struct heapnode_header header;
    struct heapnode *next;
} __attribute__((packed));

struct buddy {
    uint8_t max_order;
    uint8_t min_order;
    struct mutex mutex;
    struct heapnode **list;
};

#define MM_HEADER_SIZE      sizeof(struct heapnode_header)
#define MM_MAX_USER_SIZE    (CONFIG_EUSERHEAP - CONFIG_SUSERHEAP)
#define MM_MAX_KERNEL_SIZE  (CONFIG_EKERNELHEAP - CONFIG_SKERNELHEAP)

extern struct buddy user_buddy;
extern struct heapnode *user_buddy_list[];

extern struct buddy kernel_buddy;
extern struct heapnode *kernel_buddy_list[];

#endif
