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
