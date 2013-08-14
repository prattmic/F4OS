/*
 * Copyright (C) 2014 F4OS Authors
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

#include <dev/raw_mem.h>
#include "clocks.h"
#include "memory_map.h"

/* Master Oscillator (MHz) */
#define CLK_M_OSC_MHZ     (CLK_M_OSC/1000000)

/*
 * ADPLLS:
 *
 * Given:
 * REGM4XEN = 0
 * SLKINPHIFSEL = 0
 *
 * CLKOUT       = (M/(N+1)) * CLK_M_OSC * (1/M2)
 * CLKOUTX2     = 2 * (M/(N+1)) * CLK_M_OSC * (1/M2)
 * CLKDCOLDO    = 2 * (M/(N+1)) * CLK_M_OSC
 * CLKOUTHIF    = 2 * (M/(N+1)) * CLK_M_OSC * (1/M3)
 */

/*
 * ADPLLLJ:
 *
 * CLKOUT       = (M/(N+1)) * CLK_M_OSC * (1/M2)
 * CLKOUTLDO    = (M/(N+1)) * CLK_M_OSC * (1/M2)
 * CLKDCOLDO    = (M/(N+1)) * CLK_M_OSC
 */

/*
 * Core PLL
 *
 * CLKDCOLDO (ADPLLS): Typical 2000MHz
 * CORE_CLKOUTM4 = CLKDCOLDO/M4 (Typical 200MHz)
 * CORE_CLKOUTM5 = CLKDCOLDO/M5 (Typical 250MHz)
 * CORE_CLKOUTM6 = CLKDCOLDO/M6 (Typical 500MHz)
 */
#define COREPLL_M   1000
#define COREPLL_N   (CLK_M_OSC_MHZ-1) /* Divide down to 1Mhz */
#define COREPLL_M4  10
#define COREPLL_M5  8
#define COREPLL_M6  4

static void core_pll_config(void) {
    uint32_t reg, mask;
    struct am335x_clock_cmwkup *cmwkup =
        (struct am335x_clock_cmwkup *) AM335X_CM_WKUP_BASE;

    /* Put DPLL in bypass mode and reset register */
    raw_mem_write(&cmwkup->clkmode_dpll_core, DPLL_MN_BYP_MODE);

    /* Wait for DPLL to be in bypass mode */
    while (!(raw_mem_read(&cmwkup->idlest_dpll_core) &
            CM_IDLEST_ST_MN_BYPASS_MASK));

    /* Set M and N */
    mask = CM_CLKSEL_DPLL_M_MASK | CM_CLKSEL_DPLL_N_MASK;
    reg = COREPLL_M << CM_CLKSEL_DPLL_M_SHIFT
            | COREPLL_N << CM_CLKSEL_DPLL_N_SHIFT;
    raw_mem_set_mask(&cmwkup->clksel_dpll_core, mask, reg);

    /* Set M4, M5, M6 */
    mask = CM_HSDIVIDER_CLKOUT_DIV_MASK;
    raw_mem_set_mask(&cmwkup->div_m4_dpll_core, mask, COREPLL_M4);
    raw_mem_set_mask(&cmwkup->div_m5_dpll_core, mask, COREPLL_M5);
    raw_mem_set_mask(&cmwkup->div_m6_dpll_core, mask, COREPLL_M6);

    /* Set DPLL to locked */
    raw_mem_write(&cmwkup->clkmode_dpll_core, DPLL_LOCK_MODE);

    /* Wait for DPLL to be locked */
    while (!(raw_mem_read(&cmwkup->idlest_dpll_core) &
            CM_IDLEST_ST_DPLL_CLK_LOCK));
}

/*
 * Peripheral PLL
 *
 * CLKDCOLDO (ADPLLLJ): Typical 960MHz
 * PER_CLKOUTM2 = CLKDCOLDO/M2 (Typical 192MHz)
 */
#define PERPLL_M   960
#define PERPLL_N   (CLK_M_OSC_MHZ-1) /* Divide down to 1Mhz */
#define PERPLL_M2  5

static void per_pll_config(void) {
    uint32_t reg, mask;
    struct am335x_clock_cmwkup *cmwkup =
        (struct am335x_clock_cmwkup *) AM335X_CM_WKUP_BASE;

    /* Put DPLL in bypass mode and reset register */
    raw_mem_write(&cmwkup->clkmode_dpll_per, DPLL_MN_BYP_MODE);

    /* Wait for DPLL to be in bypass mode */
    while (!(raw_mem_read(&cmwkup->idlest_dpll_per) &
            CM_IDLEST_ST_MN_BYPASS_MASK));

    /* Set M and N */
    mask = CM_CLKSEL_DPLL_M_MASK | CM_CLKSEL_DPLL_N_MASK;
    reg = PERPLL_M << CM_CLKSEL_DPLL_M_SHIFT
            | PERPLL_N << CM_CLKSEL_DPLL_N_SHIFT;
    raw_mem_set_mask(&cmwkup->clksel_dpll_per, mask, reg);

    /* Set M2 */
    mask = CM_HSDIVIDER_CLKOUT_DIV_MASK;
    raw_mem_set_mask(&cmwkup->div_m2_dpll_per, mask, PERPLL_M2);

    /* Set DPLL to locked */
    raw_mem_write(&cmwkup->clkmode_dpll_per, DPLL_LOCK_MODE);

    /* Wait for DPLL to be locked */
    while (!(raw_mem_read(&cmwkup->idlest_dpll_per) &
            CM_IDLEST_ST_DPLL_CLK_LOCK));
}

/*
 * MPU PLL (ADPLLLJ)
 *
 * Primary Cortex-A8 clock
 *
 * CLKOUT = M/(N+1) * CLK_M_OSC * 1/M2 (Typical CONFIGURABLE)
 */
#define MPUPLL_M   CONFIG_AM335X_DEFAULT_SYSTEM_CLOCK
#define MPUPLL_N   (CLK_M_OSC_MHZ-1) /* Divide down to 1Mhz */
#define MPUPLL_M2  1

static void mpu_pll_config(void) {
    uint32_t reg, mask;
    struct am335x_clock_cmwkup *cmwkup =
        (struct am335x_clock_cmwkup *) AM335X_CM_WKUP_BASE;

    /* Put DPLL in bypass mode and reset register */
    raw_mem_write(&cmwkup->clkmode_dpll_mpu, DPLL_MN_BYP_MODE);

    /* Wait for DPLL to be in bypass mode */
    while (!(raw_mem_read(&cmwkup->idlest_dpll_mpu) &
            CM_IDLEST_ST_MN_BYPASS_MASK));

    /* Set M and N */
    mask = CM_CLKSEL_DPLL_M_MASK | CM_CLKSEL_DPLL_N_MASK;
    reg = MPUPLL_M << CM_CLKSEL_DPLL_M_SHIFT
            | MPUPLL_N << CM_CLKSEL_DPLL_N_SHIFT;
    raw_mem_set_mask(&cmwkup->clksel_dpll_mpu, mask, reg);

    /* Set M2 */
    mask = CM_HSDIVIDER_CLKOUT_DIV_MASK;
    raw_mem_set_mask(&cmwkup->div_m2_dpll_mpu, mask, MPUPLL_M2);

    /* Set DPLL to locked */
    raw_mem_write(&cmwkup->clkmode_dpll_mpu, DPLL_LOCK_MODE);

    /* Wait for DPLL to be locked */
    while (!(raw_mem_read(&cmwkup->idlest_dpll_mpu) &
            CM_IDLEST_ST_DPLL_CLK_LOCK));
}

static void display_pll_config(void) {
    /* Display currently unsupported */
    return;
}

/*
 * DDR PLL (ADPLLS)
 *
 * CLKOUT = M/(N+1) * 2 * CLK_M_OSC * 1/(2*M2) (Example 266MHz)
 */
#define DDRPLL_M   266
#define DDRPLL_N   (CLK_M_OSC_MHZ-1) /* Divide down to 1Mhz */
#define DDRPLL_M2  1

/**
 * Configure DDR PLL
 *
 * Due to the setup of N and M2, the DDR PLL M will be the CLKOUT
 * frequency in MHz.
 *
 * @param pll_m PLL Multiplier.  This will be DDR CLKOUT in MHz
 */
static void ddr_pll_config(uint16_t pll_m) {
    uint32_t reg, mask;
    struct am335x_clock_cmwkup *cmwkup =
        (struct am335x_clock_cmwkup *) AM335X_CM_WKUP_BASE;

    /* Put DPLL in bypass mode and reset register */
    raw_mem_write(&cmwkup->clkmode_dpll_ddr, DPLL_MN_BYP_MODE);

    /* Wait for DPLL to be in bypass mode */
    while (!(raw_mem_read(&cmwkup->idlest_dpll_ddr) &
            CM_IDLEST_ST_MN_BYPASS_MASK));

    /* Set M and N */
    mask = CM_CLKSEL_DPLL_M_MASK | CM_CLKSEL_DPLL_N_MASK;
    reg = pll_m << CM_CLKSEL_DPLL_M_SHIFT
            | DDRPLL_N << CM_CLKSEL_DPLL_N_SHIFT;
    raw_mem_set_mask(&cmwkup->clksel_dpll_ddr, mask, reg);

    /* Set M2 */
    mask = CM_HSDIVIDER_CLKOUT_DIV_MASK;
    raw_mem_set_mask(&cmwkup->div_m2_dpll_ddr, mask, DDRPLL_M2);

    /* Set DPLL to locked */
    raw_mem_write(&cmwkup->clkmode_dpll_ddr, DPLL_LOCK_MODE);

    /* Wait for DPLL to be locked */
    while (!(raw_mem_read(&cmwkup->idlest_dpll_ddr) &
            CM_IDLEST_ST_DPLL_CLK_LOCK));
}

void am335x_clock_config(void) {
    core_pll_config();
    per_pll_config();
    mpu_pll_config();
    display_pll_config();
    ddr_pll_config(DDRPLL_M);
}
