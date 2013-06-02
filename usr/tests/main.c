#include <stdio.h>
#include <kernel/sched.h>
#include "test.h"

extern struct test _user_start;
extern struct test _user_end;

struct test *tests = (struct test *)&_user_start;

#define NUM_TESTS ((int)(&_user_end - &_user_start))

void run_tests(void) {
    int failures = 0;

    /* Wait for stdin/stdout - getc returns a negative error when it is not connected. */
    while (getc() < 0);

    for (int i = 0; i < NUM_TESTS; i++) {
        struct test test = tests[i];

        printf("Test '%s'...", test.name);
        int ret = test.func();

        if (ret) {
            printf("FAILED with return code %d\r\n", ret);
            failures++;
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

int good_test(void) {
    return 0;
}
DEFINE_TEST("Good test", good_test);

int bad_test(void) {
    return 1;
}
DEFINE_TEST("Bad test", bad_test);
