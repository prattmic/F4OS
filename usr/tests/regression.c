#include <stdio.h>
#include <string.h>
#include "test.h"

/* strreverse would always skip the first byte when searching for the
 * end of the str, causing it to overflow when given a string containing
 * only a NULL byte */
int strreverse_buffer_overflow(char *message, int len) {
    char buf[] = {'\0', 'a', 'b', '\0'};

    strreverse(buf);

    if (buf[0] != '\0') {
        scnprintf(message, len, "buffer NULL overwritten");
        return FAILED;
    }

    if (buf[1] != 'a' || buf[2] != 'b') {
        scnprintf(message, len, "buffer contents overwritten");
        return FAILED;
    }

    return PASSED;
}
DEFINE_TEST("strreverse buffer overflow", strreverse_buffer_overflow);
