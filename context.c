/* Context switching fro F4OS!
 * Michael Pratt <michael@pratt.im> */

#include "types.h"
#include "registers.h"
#include "mem.h"
#include "context.h"

void unprivileged_test(void);

void user_prefix(void) {
    uint32_t *memory;

    /* Allocate memory for the stack */
    memory = (uint32_t *) alloc();

    /* Give unprivileged access to the allocated stack */
    *MPU_RNR = (uint32_t) (1 << 7);   /* Region 7 */
    *MPU_RBAR = (uint32_t) memory;
    /* (Enable = 1) | (SIZE = Size for 1 PG) | (B = 1) | (C = 0) | (S = 1) | (AP = 3 (all rw)) | (XN = 1) */
    *MPU_RASR = (1 << 0) | (pg_mpu_size << 1) | (1 << 16) | (0 << 17) | (1 << 18) | (3 << 24) | (1 << 28);

    /* Switch to the process stack and set it to the
     * top of the allocated memory */
    enable_psp(memory+PGSIZE);

    unprivileged_test();

    /* Raise privilege */
    asm ("svc   #0");

    disable_psp();
}
