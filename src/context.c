/* Context switching for F4OS!
 * Michael Pratt <michael@pratt.im> */

#include "types.h"
#include "registers.h"
#include "interrupt.h"
#include "mpu.h"
#include "usermode.h"
#include "task.h"
#include "semaphore.h"
#include "buddy.h"
#include "mem.h"
#include "context.h"

void user_prefix(void) {
    uint32_t *memory;

    /* Allocate memory for the stack, must be aligned to stack size for MPU */
    memory = malloc(STKSIZE*4);

    /* Give unprivileged access to the allocated stack */
    *MPU_RNR = (uint32_t) (1 << USER_MEM_REGION);
    *MPU_RBAR = (uint32_t) memory;
    *MPU_RASR = MPU_RASR_ENABLE | MPU_RASR_SIZE((mpu_size(STKSIZE*4))) | MPU_RASR_SHARE_NOCACHE_WBACK | MPU_RASR_AP_PRIV_RW_UN_RW | MPU_RASR_XN;
    /* MPU Base address must be aligned with the MPU region size */

    /* Switch to the process stack and set it to the
     * top of the allocated memory */
    enable_psp(memory+STKSIZE);

    unprivileged_test();

    /* Raise privilege */
    _svc(SVC_RAISE_PRIV);

    /* Test context switching */
    //_svc(1);

    disable_psp();
}

/* systick_handler calls pendsv to service task switching */
void pendsv_handler(void){
    uint32_t *psp_addr;

    __asm__("push {lr} \n\t");
    __asm__("cpsid i \n\t");

    psp_addr = save_context();

    curr_task->task->stack_top = psp_addr;

    switch_task();

    restore_context();

    __asm__("cpsie i \n\t");
    __asm__("pop {lr} \n\t");
    __asm__("bx lr \n\t");
}

void svc_handler(uint32_t *svc_args) {
    uint32_t svc_number;
    uint32_t return_address;

    /* Stack contains:
     * r0, r1, r2, r3, r12, r14, the return address and xPSR
     * First argument (r0) is svc_args[0] */
    svc_number = ((char *)svc_args[6])[-2];

    switch (svc_number) {
        case SVC_RAISE_PRIV: {
            /* Raise Privilege, but only if request came from the kernel */
            /* DEPRECATED: All code executed until _svc returns is privileged,
             * so raising privileges shouldn't ever be necessary. */
            return_address = svc_args[6];

            if (return_address >= (uint32_t) &_skernel && return_address < (uint32_t) &_ekernel) {
                raise_privilege();
            }
            else {
                panic_print("User code requested raised permissions.");
            }
            break;
        }
        case SVC_YIELD: {
            /* Set PendSV to yield a task */
            *SCB_ICSR |= SCB_ICSR_PENDSVSET;
            break;
        }
        case SVC_END_TASK: {
            __asm__("mov r10, sp \n\t");
            task_node *node = task_to_free;

            /* curr_task->next set to NULL after task switch */
            if (node != NULL) {
                while (node->next != NULL) {
                    node = node->next;
                }
                node->next = curr_task;
                node = node->next;
            }
            else {
                task_to_free = curr_task;
                node = task_to_free;
            }

            __asm__("push {lr} \n\t");

            remove_task(&task_list, curr_task);

            switch_task();

            node->next = NULL;

            enable_psp(curr_task->task->stack_top);

            __asm__("pop {lr} \n\t");

            __asm__("mov sp, %[ghetto] \n\t"
                    "b  restore_context \n\t"    /* Won't return */
                    ::[ghetto] "r" (ghetto_sp_save):);
            break;
        }
        case SVC_END_PERIODIC_TASK: {
            __asm__("push {lr} \n\t");

            remove_task(&task_list, curr_task);

            curr_task->task->running = 0;

            /* Reset stack */
            curr_task->task->stack_top = curr_task->task->stack_base + STKSIZE;

            switch_task();

            enable_psp(curr_task->task->stack_top);

            __asm__("pop {lr} \n\t");

            __asm__("mov sp, %[ghetto] \n\t"
                    "b  restore_context \n\t"    /* Won't return */
                    ::[ghetto] "r" (ghetto_sp_save):);
        }
        default:
            break;
    }
}

/* Makes node run immediately */
void swap_task(task_node *node) {
    register uint32_t *stack_top asm("r0");
    
    __asm__(/* If you aren't already on the psp, you are screwed, sorry */
            "str     r0, [sp, #-100] \n\t"      /* Go ahead and store r0, so I can use it */

            "mrs     r0, xpsr \n\t"
            "orr     r0, r0, #0x1000000 \n\t"
            "str     r0, [sp, #-72] \n\t"      /* Store xpsr quickly, don't want it to change */

            /* 18 words left for fp registers, which I don't save yet */

            "ldr     r0, =swap_complete \n\t"
            "str     r0, [sp, #-76] \n\t"      /* Save PC as the end of this function */

            "str     lr, [sp, #-80] \n\t"
            "str     r12, [sp, #-84] \n\t"
            "str     r3, [sp, #-88] \n\t"
            "str     r2, [sp, #-92] \n\t"
            "str     r1, [sp, #-96] \n\t"
           
            "sub     r0, sp, #100 \n\t"
            "stmfd   r0!, {r4-r11} \n\t"   /* Saves multiple registers and writes the final address back to Rn */

            "msr     psp, r0 \n\t"
            );

    curr_task->task->stack_top = stack_top;

    curr_task = node;

    enable_psp(curr_task->task->stack_top);

    __asm__("b  restore_full_context \n\t");  /* Won't return */

    __asm__("swap_complete: \n\t"
            "nop \n\t");
}
