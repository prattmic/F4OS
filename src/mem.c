/* mem.c: memory specific kernel operations */

#include "types.h"
#include "mem.h"
#include "registers.h"
#include "interrupt.h"

/* Set size bytes to value from p */
void memset32(void *p, int32_t value, uint32_t size) {
    uint32_t *end = (uint32_t *) ((uint32_t) p + size);

    /* Disallowed unaligned addresses */
    if ( (uint32_t) p % 4 ) {
        panic();
    }

    while ( (uint32_t*) p < end ) {
        *((uint32_t*)p) = value;
        p++;
    }
}
