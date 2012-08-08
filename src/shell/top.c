#include "types.h"
#include "math.h"
#include "usart.h"
#include "task.h"
#include "semaphore.h"
#include "buddy.h"
#include "stdio.h"
#include "top.h"

static uint32_t free_memory(struct buddy *buddy);

/* Display memory usage */
void top(int argc, char **argv) {
    printf("User buddy free memory: %d bytes\r\n", free_memory(&user_buddy));
    printf("Kernel buddy free memory: %d bytes\r\n", free_memory(&kernel_buddy));
}

uint32_t free_memory(struct buddy *buddy) {
    acquire(&buddy->semaphore);

    uint32_t free = 0;

    for (int i = buddy->min_order; i <= buddy->max_order; i++) {
        struct heapnode *node = buddy->list[i];
        while (node) {
            free += pow(2, i);
            node = node->next;
        }
    }

    release(&buddy->semaphore);

    return free;
}
