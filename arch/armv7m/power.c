#include <kernel/power.h>

int arch_wait_for_interrupt(void) {
    asm("wfe\n");

    return 0;
}
