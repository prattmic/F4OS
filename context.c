/* Context switching fro F4OS!
 * Michael Pratt <michael@pratt.im> */

#include "types.h"
#include "registers.h"
#include "mem.h"
#include "context.h"

void unprivileged_test(void);
void panic(void);

void user_prefix(void) {
    uint32_t *memory;

    /* Allocate memory for the stack */
    memory = (uint32_t *) alloc();

    /* Give unprivileged access to the allocated stack */
    *MPU_RNR = (uint32_t) (1 << USER_STACK_REGION);   /* Region 7 */
    *MPU_RBAR = (uint32_t) memory;
    /* (Enable = 1) | (SIZE = Size for 1 PG) | (B = 1) | (C = 0) | (S = 1) | (AP = 3 (all rw)) | (XN = 1) */
    *MPU_RASR = (1 << 0) | (pg_mpu_size << 1) | (1 << 16) | (0 << 17) | (1 << 18) | (3 << 24) | (1 << 28);

    /* Switch to the process stack and set it to the
     * top of the allocated memory */
    enable_psp(memory+PGSIZE);

    //unprivileged_test();

    /* Raise privilege */
    _svc(0);

    disable_psp();
}

void svc_handler(uint32_t *svc_args) {
    uint32_t svc_number;
    uint32_t return_address;

    /* Stack contains:
     * r0, r1, r2, r3, r12, r14, the return address and xPSR
     * First argument (r0) is svc_args[0] */
    svc_number = ((char *)svc_args[6])[-2];
    return_address = svc_args[6];

    switch (svc_number) {
        case 0x0:
            /* Raise Privilege, but only if request came from the kernel */
            if (return_address >= (uint32_t) &_skernel && return_address < (uint32_t) &_ekernel) {
                raise_privilege();
            }
            else {
                panic();
            }
            break;
        default:
            break;
    }
}
