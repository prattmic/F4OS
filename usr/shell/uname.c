#include <stdio.h>
#include <string.h>
#include "shell.h"
#include "app.h"

#if BUILD_DIRTY
#define DIRTY_MESSAGE   "~dirty"
#else
#define DIRTY_MESSAGE   ""
#endif

void uname(int argc, char **argv) {
    if (argc > 1) {
        if (!strncmp(argv[1], "-a", SHELL_ARG_BUF_MAX)) {
            printf("F4OS build %s%s %s/%s %s\r\n", BUILD_SHA, DIRTY_MESSAGE,
                    CONFIG_ARCH, CONFIG_CHIP, BUILD_TIME);
        }
        else if (!strncmp(argv[1], "-m", SHELL_ARG_BUF_MAX)) {
            printf("%s/%s\r\n", CONFIG_ARCH, CONFIG_CHIP);
        }
        else if (!strncmp(argv[1], "-r", SHELL_ARG_BUF_MAX)) {
            printf("build %s%s\r\n", BUILD_SHA, DIRTY_MESSAGE);
        }
        else {
            printf("%s: unrecognized option '%s'\r\n", argv[0], argv[1]);
        }
    }
    else {
        puts("F4OS\r\n");
    }
}
DEFINE_APP(uname)
