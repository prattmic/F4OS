/* Handles the rest of the boot process from where boot.S left off
 * F4OS
 * Michael Pratt <michael@pratt.im> */

#include "types.h"
#include "registers.h"
#include "mem.h"
#include "context.h"
#include "systick.h"
#include "heap.h"

/* From boot.S */
void panic(void);

static void clock(void) __attribute__((section(".kernel")));
static void power_led(void) __attribute__((section(".kernel")));
static void mpu_setup(void) __attribute__((section(".kernel")));

static void dont_panic(void) __attribute__((section(".kernel")));

int main(void) __attribute__((section(".kernel")));

int main(void) {
    clock();
    power_led();
    mpu_setup();
    stack_setup();
    systick_init();
    user_prefix();
    init_kheap();
    float* lol = (float *) kmalloc(sizeof(float));
    *lol = 3.14159;
    dont_panic();
    return 0;
}

/* Temporary function to prevent main from returning */
void dont_panic(void) {
    while (1) {
    }
}

#define HSE_STARTUP_TIMEOUT     (uint16_t) (0x0500)         /* Time out for HSE start up */
/* PLL Options - See RM0090 Reference Manual pg. 95 */
#define PLL_M      8            /* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N */
#define PLL_N      336
#define PLL_P      2            /* SYSCLK = PLL_VCO / PLL_P */
#define PLL_Q      7            /* USB OTG FS, SDIO and RNG Clock =  PLL_VCO / PLLQ */

/* Sets the clock */
static void clock(void) {
    /********* Reset clock registers ************/
    /* Set HSION bit */
    *RCC_CR |= (uint32_t)0x00000001;

    /* Reset CFGR register */
    *RCC_CFGR = 0x00000000;

    /* Reset HSEON, CSSON and PLLON bits */
    *RCC_CR &= (uint32_t)0xFEF6FFFF;

    /* Reset PLLCFGR register */
    *RCC_PLLCFGR = 0x24003010;

    /* Reset HSEBYP bit */
    *RCC_CR &= (uint32_t)0xFFFBFFFF;

    /* Disable all interrupts */
    *RCC_CIR = 0x00000000;

    /******* Set up the clock *************/

    volatile uint32_t StartUpCounter = 0, HSEStatus = 0;

    /* Enable HSE */
    *RCC_CR |= RCC_CR_HSEON;

    /* Wait till HSE is ready and if Time out is reached exit */
    do {
        HSEStatus = *RCC_CR & RCC_CR_HSERDY;
        StartUpCounter++;
    } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((*RCC_CR & RCC_CR_HSERDY) != 0) {
        HSEStatus = (uint32_t)0x01;
    }
    else {
        HSEStatus = (uint32_t)0x00;
    }

    if (HSEStatus == (uint32_t)0x01) {
        /* Enable high performance mode, System frequency up to 168 MHz */
        *RCC_APB1ENR |= RCC_APB1ENR_PWREN;
        *PWR_CR |= PWR_CR_VOS;  

        /* HCLK = SYSCLK / 1*/
        *RCC_CFGR |= RCC_CFGR_HPRE_DIV1;
          
        /* PCLK2 = HCLK / 2*/
        *RCC_CFGR |= RCC_CFGR_PPRE2_DIV2;

        /* PCLK1 = HCLK / 4*/
        *RCC_CFGR |= RCC_CFGR_PPRE1_DIV4;

        /* Configure the main PLL */
        /* PLL Options - See RM0090 Reference Manual pg. 95 */
        *RCC_PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
                       (RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24);

        /* Enable the main PLL */
        *RCC_CR |= RCC_CR_PLLON;

        /* Wait till the main PLL is ready */
        while((*RCC_CR & RCC_CR_PLLRDY) == 0) {
        }

        /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
        *FLASH_ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS;

        /* Select the main PLL as system clock source */
        *RCC_CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        *RCC_CFGR |= RCC_CFGR_SW_PLL;

        /* Wait till the main PLL is used as system clock source */
        while ((*RCC_CFGR & (uint32_t) RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL); {
        }
    }
    else { 
        /* If HSE fails to start-up, the application will have wrong clock configuration. */
        panic();
    }

    /* Enable the CCM RAM clock */
    *RCC_AHB1ENR |= (1 << 20);
}

/* Turns on the red LED to show the device is booting */
static void power_led() {
    /* Enable Port D Clock
    * See docs/stm32f4_ref.pdf page 110 for description of RCC_AHB1ENR */
    *RCC_AHB1ENR |= (1 << 3);

    /* Set red LED pin to output
    * See docs/stm32f4_ref.pdf page 148 for description of GPIOD_MODER */
    *GPIOD_MODER |= (1 << (14 * 2));

    /* Enable LED */
    *LED_ODR |= (1 << 14);
}

/* Enables the MPU and sets the default memory map. */
static void mpu_setup(void) {
    /* The defualt memory map sets everything as accessible only to privileged access
     * Any unprivileged accesses will need to be explicitly allowed through a region. */
    uint32_t kernel_size = mpu_size((uint32_t) (&_ekernel) - (uint32_t) (&_skernel));

    /* Set entire flash to unprivileged read only */
    *MPU_RNR = (uint32_t) (1 << FLASH_REGION);
    *MPU_RBAR = FLASH_BASE;
    *MPU_RASR = MPU_RASR_ENABLE | MPU_RASR_SIZE(19) | MPU_RASR_SHARE_CACHE_WBACK | MPU_RASR_AP_PRIV_RW_UN_RO;

    /* Set .kernel section to privileged access only */
    *MPU_RNR = (uint32_t) (1 << KERNEL_CODE_REGION);
    *MPU_RBAR = (uint32_t) (&_skernel);
    *MPU_RASR = MPU_RASR_ENABLE | MPU_RASR_SIZE(kernel_size) | MPU_RASR_SHARE_CACHE_WBACK | MPU_RASR_AP_PRIV_RW_UN_NO;

    /* Set CCM RAM (kernel stack) to privileged access only */
    *MPU_RNR = (uint32_t) (1 << KERNEL_STACK_REGION);
    *MPU_RBAR = CCMRAM_BASE;
    *MPU_RASR = MPU_RASR_ENABLE | MPU_RASR_SIZE(15) | MPU_RASR_SHARE_CACHE_WBACK | MPU_RASR_AP_PRIV_RW_UN_NO;
    
    /* For now, let every one access general peripherals, system peripherals and registers are protected. */
    *MPU_RNR = (uint32_t) (1 << PERIPH_REGION);
    *MPU_RBAR = PERIPH_BASE;
    *MPU_RASR = MPU_RASR_ENABLE | MPU_RASR_SIZE(28) | MPU_RASR_SHARE_NOCACHE_WBACK | MPU_RASR_AP_PRIV_RW_UN_RW | MPU_RASR_XN;


    /* Enable the MPU and allow privileged access to the background map */
    *MPU_CTRL |= MPU_CTRL_ENABLE | MPU_CTRL_PRIVDEFENA;

    /* Enable the memory management fault */
    *SCB_SHCSR |= SCB_SHCSR_MEMFAULTENA;
}
