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
