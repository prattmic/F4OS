#include <stdio.h>
#include "trace_test.h"

void trace_test(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s stuff to send as trace\r\n", argv[0]);
        return;
    }

    for (int i = 1; i < argc; i++) {
        printf("Sending trace '%s'...\r\n", argv[i]);

        TRACE(argv[i]);
    }
}
