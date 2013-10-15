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
