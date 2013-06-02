#include <stdint.h>
#include <stdio.h>
#include <kernel/semaphore.h>
#include <mm/mm.h>
#include "app.h"

/* Display memory usage */
void top(int argc, char **argv) {
    printf("User buddy free memory: %d bytes\r\n", mm_space());
    printf("Kernel buddy free memory: %d bytes\r\n", mm_kspace());
}
DEFINE_APP(top)
