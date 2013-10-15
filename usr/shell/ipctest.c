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

#include <stddef.h>
#include <stdio.h>
#include <kernel/sched.h>
#include <dev/resource.h>
#include <dev/shared_mem.h>
#include "app.h"

void memreader(void);

void ipctest(int argc, char **argv) {
    if (argc != 1) {
        printf("Usage: %s\n", argv[0]);
        return;
    }

    rd_t memrd = open_shared_mem();
    if (memrd < 0) {
        printf("Error: unable to open shared mem.\r\n");
    }

    printf("WRITING MEM.\r\n");

    swrite(memrd, "THIS IS A TEST OF SHARED MEMORY REGIONS N STUFF.");

    printf("READING MEM.\r\n");
    new_task(&memreader, 5, 0);
}
DEFINE_APP(ipctest)

void memreader(void) {
    char buf[16];
    rd_t memrd = curr_task->resource_data.top_rd - 1;

    read(memrd, buf, 10);
    buf[10] = 0x00;

    puts(buf);

    close(memrd);
}
