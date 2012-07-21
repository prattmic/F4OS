/* Handles the rest of the boot process from where boot.S left off
 * F4OS
 * Michael Pratt <michael@pratt.im> */

#include "types.h"
#include "registers.h"
#include "interrupt.h"
#include "task.h"
#include "mem.h"
#include "context.h"
#include "systick.h"
#include "semaphore.h"
#include "buddy.h"
#include "usermode.h"
#include "usart.h"
#include "spi.h"
#include "tim.h"

static void clock(void) __attribute__((section(".kernel")));
static void power_led(void) __attribute__((section(".kernel")));

static void dont_panic(void) __attribute__((section(".kernel")));

int main(void) __attribute__((section(".kernel")));

int main(void) {
    clock();
    power_led();
    //mpu_setup();
    init_heap();
    init_usart();
    //init_spi();
    init_timer();

    puts("\r\n\r\n\r\nWelcome to...\r\n");

    puts("\r\n"
         "88888888888      ,d8      ,ad8888ba,     ad88888ba   \r\n"
         "88             ,d888     d8\"\'    `\"8b   d8\"     \"8b  \r\n"
         "88           ,d8\" 88    d8\'        `8b  Y8,          \r\n"
         "88aaaaa    ,d8\"   88    88          88  `Y8aaaaa,    \r\n"
         "88\"\"\"\"\"  ,d8\"     88    88          88    `\"\"\"\"\"8b,  \r\n"
         "88       8888888888888  Y8,        ,8P          `8b  \r\n"
         "88                88     Y8a.    .a8P   Y8a     a8P  \r\n"
         "88                88      `\"Y8888Y\"\'     \"Y88888P\"   \r\n"
         "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n"
         "\r\n");

    led_tasks();

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
    /* Enable the FPU */
    *SCB_CPACR |= (0xf << 20);
    /* Enable floating point state preservation */
    *FPU_CCR |= FPU_CCR_ASPEN;

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

    volatile uint32_t startup_count = 0, HSE_status = 0;

    /* Enable HSE */
    *RCC_CR |= RCC_CR_HSEON;

    /* Wait till HSE is ready and if Time out is reached exit */
    do {
        HSE_status = *RCC_CR & RCC_CR_HSERDY;
        startup_count++;
    } while((HSE_status == 0) && (startup_count != HSE_STARTUP_TIMEOUT));

    if ((*RCC_CR & RCC_CR_HSERDY) != 0) {
        HSE_status = (uint32_t)0x01;
    }
    else {
        HSE_status = (uint32_t)0x00;
    }

    if (HSE_status == (uint32_t)0x01) {
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
        //*FLASH_ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS;
        *FLASH_ACR = FLASH_ACR_LATENCY_5WS;

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

    /* Enable Bus and Usage Faults */
    *SCB_SHCSR |= SCB_SHCSR_BUSFAULTENA;
    *SCB_SHCSR |= SCB_SHCSR_USEFAULTENA;
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
