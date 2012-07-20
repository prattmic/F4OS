#include "types.h"
#include "registers.h"
#include "task.h"
#include "mem.h"
#include "mpu.h"

/* Enables the MPU and sets the default memory map. */
/* MPU Base address must be aligned with the MPU region size */
void mpu_setup(void) {
    /* The defualt memory map sets everything as accessible only to privileged access
     * Any unprivileged accesses will need to be explicitly allowed through a region. */
    uint32_t kernel_size = mpu_size((uint32_t) (&_ekernel) - (uint32_t) (&_skernel));

    /* Base - Privileged permissions only */
    *MPU_RNR = (uint32_t) (1 << BASE_REGION);
    *MPU_RBAR = MEMORY_BASE;
    *MPU_RASR = MPU_RASR_ENABLE | MPU_RASR_SIZE(31) | MPU_RASR_SHARE_CACHE_WBACK | MPU_RASR_AP_PRIV_RW_UN_RW;

    /* Vectors and flash */
    /* This shouldn't be required, but is */
    *MPU_RNR = (uint32_t) (1 << VECTFLASH_REGION);
    *MPU_RBAR = MEMORY_BASE;
    *MPU_RASR = MPU_RASR_ENABLE | MPU_RASR_SIZE(27) | MPU_RASR_SHARE_CACHE_WBACK | MPU_RASR_AP_PRIV_RW_UN_RO;

    /* .text memory */
    *MPU_RNR = (uint32_t) (1 << RAM_REGION);
    *MPU_RBAR = RAM_BASE;
    *MPU_RASR = MPU_RASR_ENABLE | MPU_RASR_SIZE(15) | MPU_RASR_SHARE_CACHE_WBACK | MPU_RASR_AP_PRIV_RW_UN_NO | MPU_RASR_XN;

    /* Private peripherals */
    *MPU_RNR = (uint32_t) (1 << PRIV_PERIPH_REGION);
    *MPU_RBAR = PRIV_PERIPH_BASE;
    *MPU_RASR = MPU_RASR_ENABLE | MPU_RASR_SIZE(28) | MPU_RASR_SHARE_NOCACHE_WBACK | MPU_RASR_AP_PRIV_RW_UN_NO | MPU_RASR_XN;

    /* .kernel section */
    /* This doesn't work, probably related to above */
    *MPU_RNR = (uint32_t) (1 << KERNEL_CODE_REGION);
    *MPU_RBAR = (uint32_t) &_skernel;
    *MPU_RASR = MPU_RASR_ENABLE | MPU_RASR_SIZE(kernel_size) | MPU_RASR_SHARE_CACHE_WBACK | MPU_RASR_AP_PRIV_RW_UN_NO;

    /* .kernel memory */
    *MPU_RNR = (uint32_t) (1 << KERNEL_MEM_REGION);
    *MPU_RBAR = CCMRAM_BASE;
    *MPU_RASR = MPU_RASR_ENABLE | MPU_RASR_SIZE(15) | MPU_RASR_SHARE_CACHE_WBACK | MPU_RASR_AP_PRIV_RW_UN_NO | MPU_RASR_XN;

    /* Enable the memory management fault */
    *SCB_SHCSR |= SCB_SHCSR_MEMFAULTENA;

    /* Enable the MPU and allow privileged access to the background map */
    *MPU_CTRL |= MPU_CTRL_ENABLE ;//| MPU_CTRL_PRIVDEFENA;
}

void mpu_stack_set(uint32_t *stack_base) {
    /* Give unprivileged access to the allocated stack */
    *MPU_RNR = (uint32_t) (1 << USER_MEM_REGION);
    *MPU_RBAR = (uint32_t) stack_base;
    *MPU_RASR = MPU_RASR_ENABLE | MPU_RASR_SIZE((mpu_size(STKSIZE*4))) | MPU_RASR_SHARE_NOCACHE_WBACK | MPU_RASR_AP_PRIV_RW_UN_RW | MPU_RASR_XN;
    /* MPU Base address must be aligned with the MPU region size */
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
