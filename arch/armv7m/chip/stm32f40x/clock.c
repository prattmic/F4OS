#include <stdint.h>
#include <arch/chip.h>
#include <arch/system.h>
#include <arch/chip/registers.h>
#include <kernel/fault.h>

#define HSE_STARTUP_TIMEOUT (0x0500)         /* Time out for HSE start up */
/* PLL Options - See RM0090 Reference Manual pg. 95 */
/* We set PLL_M to the board's defined oscillator frequency */
#define PLL_M      CONFIG_STM32_OSC_FREQ   /* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N */
#define PLL_N      336
#define PLL_P      2            /* SYSCLK = PLL_VCO / PLL_P */
#define PLL_Q      7            /* USB OTG FS, SDIO and RNG Clock =  PLL_VCO / PLLQ */

void init_clock(void) {
    /********* Reset clock registers ************/
    /* Set HSION bit */
    *RCC_CR |= RCC_CR_HSION;

    /* Reset CFGR register */
    *RCC_CFGR = 0x00000000;

    /* Reset HSEON, CSSON and PLLON bits */
    *RCC_CR &= ~(RCC_CR_HSEON | RCC_CR_CSSON | RCC_CR_PLLON);

    /* Reset PLLCFGR register */
    *RCC_PLLCFGR = RCC_PLLCFGR_RESET;

    /* Reset HSEBYP bit */
    *RCC_CR &= ~(RCC_CR_HSEBYP);

    /* Disable all interrupts */
    *RCC_CIR = 0x00000000;

    /******* Set up the clock *************/

    volatile uint32_t startup_count = 0, HSE_status = 0;

    /* Enable HSE */
    *RCC_CR |= RCC_CR_HSEON;

    /* Wait till HSE is ready and if Time out is reached exit */
    do {
        HSE_status = *RCC_CR & RCC_CR_HSERDY;
        startup_count++;
    } while((HSE_status == 0) && (startup_count != HSE_STARTUP_TIMEOUT));

    if ((*RCC_CR & RCC_CR_HSERDY) != 0) {
        HSE_status = 0x01;
    }
    else {
        HSE_status = 0x00;
    }

    if (HSE_status == 0x01) {
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
        while((*RCC_CR & RCC_CR_PLLRDY) == 0);

        /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
        //*FLASH_ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS;
        *FLASH_ACR = FLASH_ACR_LATENCY(5);

        /* Select the main PLL as system clock source */
        *RCC_CFGR &= ~(RCC_CFGR_SW_M);
        *RCC_CFGR |= RCC_CFGR_SW_PLL;

        /* Wait till the main PLL is used as system clock source */
        while ((*RCC_CFGR & RCC_CFGR_SWS_M) != RCC_CFGR_SWS_PLL); {
        }
    }
    else {
        /* If HSE fails to start-up, the application will have wrong clock configuration. */
        panic();
    }

    /* Enable the CCM RAM clock */
    *RCC_AHB1ENR |= RCC_AHB1ENR_CCMDATARAMEN;
}
