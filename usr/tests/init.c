#include <stdio.h>
#include <kernel/init.h>
#include "test.h"

int basic_initializer_test_magic;   /* In bss, so it's == 0 on startup */
int order_initializer_test_magic;

int basic_test(void) {
    basic_initializer_test_magic = 0xdeadbeef;
    return 0;
}
LATE_INITIALIZER(basic_test)

int early_order_test(void) {
    order_initializer_test_magic = 1;
    return 0;
}
EARLY_INITIALIZER(early_order_test)

int core_order_test(void) {
    if(order_initializer_test_magic == 1)
        order_initializer_test_magic = 2;
    else
        order_initializer_test_magic = -1;
    return 0;
}
CORE_INITIALIZER(core_order_test)

int late_order_test(void) {
    if(order_initializer_test_magic == 2)
        order_initializer_test_magic = 3;
    else
        order_initializer_test_magic = -2;
    return 0;
}
LATE_INITIALIZER(late_order_test)

int basic_initializer_test(char *message, int len) {
    if(basic_initializer_test_magic == 0xdeadbeef)
        return PASSED;
    scnprintf(message, len, "Value was 0x%x, should have been 0xdeadbeef", basic_initializer_test_magic);
    return FAILED;
}
DEFINE_TEST("Simple initializer test", basic_initializer_test)

int initializer_order_test(char *message, int len) {
    if(order_initializer_test_magic == 3) /* The error could be made more specific... */
        return PASSED;
    scnprintf(message, len, "Value was %d, should have been 3", order_initializer_test_magic);
    return FAILED;
}
DEFINE_TEST("Initializer ordering test", initializer_order_test)
