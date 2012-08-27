/* The main function in this file is called by the scheduler
 * before beginning task switching.  The end user should use
 * this function to create tasks that should run at boot, or
 * to perform any other actions that need to take place at the
 * end of boot. 
 *
 * main() is defined in kernel/sched.h */

#include <stddef.h>
#include <stdio.h>
#include <dev/resource.h>
#include <dev/shared_mem.h>
#include <kernel/sched.h>

static void unit_tests(void);
static void ipctest(void);
static void memreader(void);

resource *test_mem = NULL;
int ipc_success = 0;


void main(void) {
    new_task(&unit_tests, 1, 0);
}

void unit_tests(void) {
    printf("Print Test...Test passed.\r\n");

    printf("IPC Test...");
    ipctest();
}

void ipctest() {
    rd_t memrd = open_shared_mem();
    swrite(memrd, "Test passed.\r\n");
    test_mem = curr_task->task->resources[memrd];
    new_task(&memreader, 5, 0);
}

void memreader(void) {
    char buf[16];
    if(test_mem == NULL) {
        return;
    }
    add_resource(curr_task->task, test_mem);
    rd_t memrd = curr_task->task->top_rd - 1;

    read(memrd, buf, 15);
    buf[15] = 0x00;

    puts(buf);

    close(memrd);
}
