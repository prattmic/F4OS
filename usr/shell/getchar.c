#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "app.h"

/* Get a number of characters */
void getchar(int argc, char **argv) {
    int num_chars = 1;
    if (argc > 1) {
        num_chars = atoi(argv[1]);
    }
    printf("Getting %d char%s\r\n", num_chars, num_chars > 1 ? "s" : "");
    for (int i = 0; i < num_chars; i++) {
        int c = getc();
        printf("\t%d: 0x%x %d\r\n", i, c, c);
    }
}
DEFINE_APP(getchar)
