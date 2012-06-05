/* mem.c: memory specific kernel operations */

#include "types.h"
#include "mem.h"

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

uint16_t mpu_size(uint32_t size) {
    /* Calculates the best region size for a given actual size.
     * the MPU register takes a value N, where 2^(N+1) is the
     * region size, where 4 <= N < 32. */
    uint32_t region = 32;/* Note this, is 2^5.  We start N at 4 because the resulting size is 2^(N+1) */
    uint16_t N = 4;      /* This is the minimum setting */

    while (region < size) {
        region <<= 1;
        N += 1;

        if (N >= 31) {  /* 31 is the max value for N */
            break;
        }
    }

    return N;
}