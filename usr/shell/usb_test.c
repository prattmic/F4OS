#include <stdio.h>

void usb_test(int argc, char **argv) {
    char c;

    fprintf(1, "Press q to quit.\r\n");

    while (1) {
        c = fgetc(1);

        if (c == 'q') {
            return;
        }

        fputc(1, c);
    }
}
