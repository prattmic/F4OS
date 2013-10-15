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
#include <kernel/system.h>
#include <string.h>

#include "app.h"

static const char *usage = "Usage:\r\n"         \
"system_ctl {<system name>,ls} <option>\r\n"    \
"currently no options, just ls systems\r\n";

void system_ctl(int argc, char **argv) {
    if(argc == 1) {
        printf("%s\r\n", usage);
        return;
    }

    if(!strncmp("ls", argv[1], 3)) {
        struct system *sys;
        struct obj *curr;

        curr = collection_iter(&systems);
        while(curr) {
            sys = to_system(curr);
            printf("%s\r\n", sys->name);
            curr = collection_next(&systems);
        }
    }
    else {
        struct system *sys;
        struct obj *sysobj, *curr_cls_obj;

        sysobj = get_system_by_name(argv[1]);
        if(!sysobj) {
            printf("Could not find system named \"%s\"\r\n", argv[1]);
            return;
        }

        sys = to_system(sysobj);
        curr_cls_obj = collection_iter(&sys->classes);
        while(curr_cls_obj) {
            printf("%s\r\n", curr_cls_obj->name);
            curr_cls_obj = collection_next(&sys->classes);
        }
        return;
    }
}
DEFINE_APP(system_ctl)
