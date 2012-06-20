/* mem.c: memory specific kernel operations */

#include "types.h"
#include "mem.h"
#include "registers.h"

void panic(void);

/* Set size bytes to value from p */
void memset32(uint32_t *p, int32_t value, uint32_t size) {
    uint32_t *end = p + size;

    /* Disallowed unaligned addresses */
    if ( (uint32_t) p % 4 ) {
        panic();
    }

    while ( p < end ) {
        *p = value;
        p++;
    }
}
