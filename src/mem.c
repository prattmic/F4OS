/* mem.c: memory specific kernel operations */

#include "types.h"
#include "registers.h"
#include "interrupt.h"
#include "task.h"
#include "mem.h"

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

void create_context(task_ctrl* task, void (*lptr)(void)) {
    asm volatile("mov     r5, #0                                                        \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  stmdb   %[stack]!, {r5}                                                     \n\
                  ldr.w   r5, =0x01000000 /* Thumb state bit must be set */             \n\
                  stmdb   %[stack]!, {r5}   /* xPSR */                                        \n\
                  mov     r5, #0                                                        \n\
                  stmdb   %[stack]!, {%[pc]}   /* PC */                                       \n\
                  stmdb   %[stack]!, {%[lr]}   /* LR */                                       \n\
                  stmdb   %[stack]!, {r5}   /* R12 */                                         \n\
                  stmdb   %[stack]!, {r5}   /* R3 */                                          \n\
                  stmdb   %[stack]!, {r5}   /* R2 */                                          \n\
                  stmdb   %[stack]!, {r5}   /* R1 */                                          \n\
                  stmdb   %[stack]!, {r5}   /* R0 */                                          \n\
                  stmdb   %[stack]!, {r5}   /* R11 */                                         \n\
                  stmdb   %[stack]!, {r5}   /* R10 */                                         \n\
                  stmdb   %[stack]!, {r5}   /* R9 */                                          \n\
                  stmdb   %[stack]!, {r5}   /* R8 */                                          \n\
                  stmdb   %[stack]!, {%[frame]}   /* R7 - Frame Pointer*/                     \n\
                  stmdb   %[stack]!, {r5}   /* R6 */                                          \n\
                  stmdb   %[stack]!, {r5}   /* R5 */                                          \n\
                  stmdb   %[stack]!, {r5}   /* R4 */"
                  :[stack] "+r" (task->stack_top) /* Output */
                  :[pc] "r" (task->fptr), [lr] "r" (lptr), [frame] "r" (task->stack_base)   /* Input */
                  :"r5"   /* Clobber */);

}
