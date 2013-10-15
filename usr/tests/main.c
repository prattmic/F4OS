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
#include "test.h"

extern struct test _user_start;
extern struct test _user_end;

struct test *tests = (struct test *)&_user_start;

#define NUM_TESTS ((int)(&_user_end - &_user_start))

#define MESSAGE_LEN 128

void run_tests(void) {
    int failures = 0;

    /* Wait for stdin/stdout - getc returns a negative error when it is not connected. */
    while (getc() < 0);

    for (int i = 0; i < NUM_TESTS; i++) {
        struct test test = tests[i];
        char message[MESSAGE_LEN] = {'\0'};

        printf("Test '%s'...", test.name);

        /* Call test, but don't allow the last byte in
         * message to be set */
        int ret = test.func(message, sizeof(message)-1);

        if (ret) {
            failures++;

            printf("FAILED");

            if (message[0]) {
                printf(" - '%s'\r\n", message);
            }
            else {
                printf("\r\n");
            }
        }
        else {
            printf("PASSED\r\n");
        }
    }

    printf("%d total failures\r\n", failures);
}

void main(void) {
    new_task(&run_tests, 1, 0);
}
