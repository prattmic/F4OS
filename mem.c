/* mem.c: memory specific kernel operations */

#include "types.h"

uint16_t mpu_size(uint32_t size) __attribute__((section(".kernel")));

struct stacklist {
    uint32_t *top;
    uint32_t size;
    struct stacklist *next;
};

struct stacklist freelist = { (uint32_t *) 0x2001C000,
                              (uint32_t)   256,
                              (struct stacklist *) 0x0};

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
