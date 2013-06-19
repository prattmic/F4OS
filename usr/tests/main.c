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
