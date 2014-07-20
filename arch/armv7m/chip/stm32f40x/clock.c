/*
 * Copyright (C) 2013, 2014 F4OS Authors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdint.h>
#include <arch/chip.h>
#include <arch/system.h>
#include <arch/chip/rcc.h>
#include <dev/raw_mem.h>
#include <kernel/fault.h>

#define SYS_CLOCK_MHZ   (CONFIG_SYS_CLOCK/1000000)

#define HSE_STARTUP_TIMEOUT (0x0500)         /* Time out for HSE start up */
/* PLL Options - See RM0090 Reference Manual pg. 95 */
/* We set PLL_M to the board's defined oscillator frequency */
#define PLL_M      CONFIG_STM32_OSC_FREQ   /* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N */
#define PLL_N      336
#define PLL_P      (PLL_N / SYS_CLOCK_MHZ) /* SYSCLK = PLL_VCO (= PLL_N) / PLL_P */
#define PLL_Q      7            /* USB OTG FS, SDIO and RNG Clock =  PLL_VCO / PLLQ */

/* PLL_P can only be 2, 4, 6, or 8 */
#if PLL_P != 2 && PLL_P != 4 && PLL_P != 6 && PLL_P != 8
#error Unable to compute valid PLL_P
#endif

void init_clock(void) {
    struct stm32f4_rcc_regs *regs = rcc_get_regs();

    /********* Reset clock registers ************/
    /* Set HSION bit */
    raw_mem_set_bits(&regs->CR, RCC_CR_HSION);

    /* Reset CFGR register */
    raw_mem_write(&regs->CFGR, 0x00000000);

    /* Reset HSEON, CSSON and PLLON bits */
    raw_mem_clear_bits(&regs->CR, RCC_CR_HSEON | RCC_CR_CSSON | RCC_CR_PLLON);

    /* Reset PLLCFGR register */
    raw_mem_write(&regs->PLLCFGR, RCC_PLLCFGR_RESET);

    /* Reset HSEBYP bit */
    raw_mem_clear_bits(&regs->CR, RCC_CR_HSEBYP);

    /* Disable all interrupts */
    raw_mem_write(&regs->CIR, 0x00000000);

    /******* Set up the clock *************/

    volatile uint32_t startup_count = 0, HSE_status = 0;

    /* Enable HSE */
    raw_mem_set_bits(&regs->CR, RCC_CR_HSEON);

    /* Wait till HSE is ready and if Time out is reached exit */
    do {
        HSE_status = raw_mem_read(&regs->CR) & RCC_CR_HSERDY;
        startup_count++;
    } while((HSE_status == 0) && (startup_count != HSE_STARTUP_TIMEOUT));

    if ((raw_mem_read(&regs->CR) & RCC_CR_HSERDY) != 0) {
        HSE_status = 0x01;
    }
    else {
        HSE_status = 0x00;
    }

    if (HSE_status == 0x01) {
        /* Enable high performance mode, System frequency up to 168 MHz */
        rcc_set_clock_enable(STM32F4_PERIPH_PWR, 1);
        *PWR_CR |= PWR_CR_VOS;

        /* HCLK = SYSCLK / 1*/
        raw_mem_set_bits(&regs->CFGR, RCC_CFGR_HPRE_DIV1);

        /* PCLK2 = HCLK / 2*/
        raw_mem_set_bits(&regs->CFGR, RCC_CFGR_PPRE2_DIV2);

        /* PCLK1 = HCLK / 4*/
        raw_mem_set_bits(&regs->CFGR, RCC_CFGR_PPRE1_DIV4);

        /* Configure the main PLL */
        /* PLL Options - See RM0090 Reference Manual pg. 95 */
        raw_mem_write(&regs->PLLCFGR, PLL_M | (PLL_N << 6) |
                        (((PLL_P >> 1) -1) << 16) |
                        (RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24));

        /* Enable the main PLL */
        raw_mem_set_bits(&regs->CR, RCC_CR_PLLON);

        /* Wait till the main PLL is ready */
        while((raw_mem_read(&regs->CR) & RCC_CR_PLLRDY) == 0);

        /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
        //*FLASH_ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS;
        *FLASH_ACR = FLASH_ACR_LATENCY(5);

        /* Select the main PLL as system clock source */
        raw_mem_set_mask(&regs->CFGR, RCC_CFGR_SW_M, RCC_CFGR_SW_PLL);

        /* Wait till the main PLL is used as system clock source */
        while ((raw_mem_read(&regs->CFGR) & RCC_CFGR_SWS_M) != RCC_CFGR_SWS_PLL);
    }
    else {
        /* If HSE fails to start-up, the application will have wrong clock configuration. */
        panic();
    }

    /* Enable the CCM RAM clock */
    rcc_set_clock_enable(STM32F4_PERIPH_CCMRAM, 1);
}
