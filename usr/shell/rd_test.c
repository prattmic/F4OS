#include <stdio.h>

void rd_test(int argc, char **argv) {
    char c;

    fprintf(stderr, "Press q to quit.\r\n");

    while (1) {
        c = fgetc(stderr);

        if (c == 'q') {
            return;
        }

        fputc(stderr, c);
    }
}
