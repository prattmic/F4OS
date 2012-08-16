#include "shell_header.h"
#include "shared_mem.h"

resource *test_mem = NULL;

void ipctest(int argc, char **argv) {
    if (argc != 1) {
        printf("Usage: %s\n", argv[0]);
        return;
    }
    rd_t memrd = open_shared_mem();
    printf("WRITING MEM.\r\n");
    swrite(memrd, "THIS IS A TEST OF SHARED MEMORY REGIONS N STUFF.");
    test_mem = curr_task->task->resources[memrd];
    printf("READING MEM.\r\n");
    new_task(&memreader, 5, 0);
}

void memreader(void) {
    char buf[16];
    if(test_mem == NULL) {
        return;
    }
    add_resource(curr_task->task, test_mem);
    rd_t memrd = curr_task->task->top_rd - 1;
    read(memrd, buf, 10);
    buf[10] = 0x00;
    puts(buf);
}
