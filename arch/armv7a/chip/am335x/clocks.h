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

#ifndef ARCH_ARMV7A_CHIP_AM335X_CLOCKS_H_INCLUDED
#define ARCH_ARMV7A_CHIP_AM335X_CLOCKS_H_INCLUDED

#include <stdint.h>

struct am335x_clock_cmper {
    uint32_t l4ls_clkstctrl;
    uint32_t l3s_clkstctrl;
    uint32_t l4fw_clkstctrl;
    uint32_t l3_clkstctrl;
    uint32_t cpgmac0_clkctrl;
    uint32_t lcdc_clkctrl;
    uint32_t usb0_clkctrl;
    uint32_t tptc0_clkctrl;
    uint32_t emif_clkctrl;
    uint32_t ocmcram_clkctrl;
    uint32_t gpmc_clkctrl;
    uint32_t mcasp0_clkctrl;
    uint32_t uart5_clkctrl;
    uint32_t mmc0_clkctrl;
    uint32_t elm_clkctrl;
    uint32_t i2c2_clkctrl;
    uint32_t i2c1_clkctrl;
    uint32_t spi0_clkctrl;
    uint32_t spi1_clkctrl;
    uint32_t l4ls_clkctrl;
    uint32_t l4fw_clkctrl;
    uint32_t mcasp1_clkctrl;
    uint32_t uart1_clkctrl;
    uint32_t uart2_clkctrl;
    uint32_t uart3_clkctrl;
    uint32_t uart4_clkctrl;
    uint32_t timer7_clkctrl;
    uint32_t timer2_clkctrl;
    uint32_t timer3_clkctrl;
    uint32_t timer4_clkctrl;
    uint32_t gpio1_clkctrl;
    uint32_t gpio2_clkctrl;
    uint32_t gpio3_clkctrl;
    uint32_t tpcc_clkctrl;
    uint32_t dcan0_clkctrl;
    uint32_t dcan1_clkctrl;
    uint32_t epwmss1_clkctrl;
    uint32_t emif_fw_clkctrl;
    uint32_t epwmss0_clkctrl;
    uint32_t epwmss2_clkctrl;
    uint32_t l3_instr_clkctrl;
    uint32_t l3_clkctrl;
    uint32_t ieee5000_clkctrl;
    uint32_t pru_icss_clkctrl;
    uint32_t timer5_clkctrl;
    uint32_t timer6_clkctrl;
    uint32_t mmc1_clkctrl;
    uint32_t mmc2_clkctrl;
    uint32_t tptc1_clkctrl;
    uint32_t tptc2_clkctrl;
    uint32_t spinlock_clkctrl;
    uint32_t mailbox0_clkctrl;
    uint32_t l4hs_clkstctrl;
    uint32_t l4hs_clkctrl;
    uint32_t ocpwp_l3_clkstctrl;
    uint32_t ocpwp_clkctrl;
    uint32_t pru_icss_clkstctrl;
    uint32_t cpsw_clkstctrl;
    uint32_t lcdc_clkstctrl;
    uint32_t clkdiv32k_clkctrl;
    uint32_t clk_24mhz_clkstctrl;
};

struct am335x_clock_cmwkup {
    uint32_t wkup_clkstctrl;
    uint32_t wkup_control_clkctrl;
    uint32_t wkup_gpio0_clkctrl;
    uint32_t wkup_l4wkup_clkctrl;
    uint32_t wkup_timer0_clkctrl;
    uint32_t wkup_debugss_clkctrl;
    uint32_t l3_aon_clkstctrl;
    uint32_t autoidle_dpll_mpu;
    uint32_t idlest_dpll_mpu;
    uint32_t ssc_deltamstep_dpll_mpu;
    uint32_t ssc_modfreqdiv_dpll_mpu;
    uint32_t clksel_dpll_mpu;
    uint32_t autoidle_dpll_ddr;
    uint32_t idlest_dpll_ddr;
    uint32_t ssc_deltamstep_dpll_ddr;
    uint32_t ssc_modfreqdiv_dpll_ddr;
    uint32_t clksel_dpll_ddr;
    uint32_t autoidle_dpll_disp;
    uint32_t idlest_dpll_disp;
    uint32_t ssc_deltamstep_dpll_disp;
    uint32_t ssc_modfreqdiv_dpll_disp;
    uint32_t clksel_dpll_disp;
    uint32_t autoidle_dpll_core;
    uint32_t idlest_dpll_core;
    uint32_t ssc_deltamstep_dpll_core;
    uint32_t ssc_modfreqdiv_dpll_core;
    uint32_t clksel_dpll_core;
    uint32_t autoidle_dpll_per;
    uint32_t idlest_dpll_per;
    uint32_t ssc_deltamstep_dpll_per;
    uint32_t ssc_modfreqdiv_dpll_per;
    uint32_t clkdcoldo_dpll_per;
    uint32_t div_m4_dpll_core;
    uint32_t div_m5_dpll_core;
    uint32_t clkmode_dpll_mpu;
    uint32_t clkmode_dpll_per;
    uint32_t clkmode_dpll_core;
    uint32_t clkmode_dpll_ddr;
    uint32_t clkmode_dpll_disp;
    uint32_t clksel_dpll_per;
    uint32_t div_m2_dpll_ddr;
    uint32_t div_m2_dpll_disp;
    uint32_t div_m2_dpll_mpu;
    uint32_t div_m2_dpll_per;
    uint32_t wkup_wkup_m3_clkctrl;
    uint32_t wkup_uart0_clkctrl;
    uint32_t wkup_i2c0_clkctrl;
    uint32_t wkup_adc_tsc_clkctrl;
    uint32_t wkup_smartreflex0_clkctrl;
    uint32_t wkup_timer1_clkctrl;
    uint32_t wkup_smartreflex1_clkctrl;
    uint32_t l4_wkup_aon_clkstctrl;
    uint32_t wkup_wdt1_clkctrl;
    uint32_t div_m6_dpll_core;
};

struct am335x_clock_cmdpll {
    uint32_t res1;
    uint32_t clksel_timer7_clk;
    uint32_t clksel_timer2_clk;
    uint32_t clksel_timer3_clk;
    uint32_t clksel_timer4_clk;
    uint32_t cm_mac_clksel;
    uint32_t clksel_timer5_clk;
    uint32_t clksel_timer6_clk;
    uint32_t cm_cpts_rft_clksel;
    uint32_t clksel_timer1ms_clk;
    uint32_t clksel_gfx_fclk;
    uint32_t clksel_pru_icss_ocp_clk;
    uint32_t clksel_lcdc_pixel_clk;
    uint32_t clksel_wdt1_clk;
    uint32_t clksel_gpio0_dbclk;
};

struct am335x_clock_cmmpu {
    uint32_t mpu_clkstctrl;
    uint32_t mpu_mpu_clkctrl;
};

struct am335x_clock_cmdevice {
    uint32_t clkout_ctrl;
};

struct am335x_clock_cmrtc {
    uint32_t rtc_clkctrl;
    uint32_t clkstctrl;
};

struct am335x_clock_cmgfx {
    uint32_t gfx_l3_clkstctrl;
    uint32_t gfx_gfx_clkctrl;
    uint32_t res1;
    uint32_t gfx_l4ls_gfx_clkstctrl;
    uint32_t gfx_mmucfg_clkctrl;
    uint32_t gfx_mmudata_clkctrl;
};

struct am335x_clock_cmcefuse {
    uint32_t cefuse_clkstctrl;
    uint32_t res1[11];
    uint32_t cefuse_cefuse_clkctl;
};

struct am335x_clock_prmirq {
    uint32_t revision;
    uint32_t irqstatus_mpu;
    uint32_t irqenable_mpu;
    uint32_t irqstatus_m3;
    uint32_t irqenable_m3;
};

/* CLKMODE register DPLL_EN modes */
enum dpll_en_modes {
    DPLL_MN_BYP_MODE = 4,
    DPLL_LP_BYP_MODE,
    DPLL_FR_BYP_MODE,
    DPLL_LOCK_MODE,
};

/* Clock module module modes */
enum cm_module_modes {
    CM_MODULEMODE_DISABLED = 0,
    CM_MODULEMODE_ENABLED = 0x2,
    CM_MODULEMODE = 0x3,
};

/* Clock idle status */
enum cm_idle_status {
    CM_IDLEST_FUNC = 0,
    CM_IDLEST_TRANS = (1 << 16),
    CM_IDLEST_IDLE = (2 << 16),
    CM_IDLEST_DIS = (3 << 16),
    CM_IDLEST = (3 << 16),
};

#define CM_IDLEST_ST_MN_BYPASS_MASK     (1 << 8)    /* DPLL in MN Bypass */
#define CM_IDLEST_ST_DPLL_CLK_LOCK      (1 << 0)    /* DPLL in Locked */

/* DPLL M/N masks and shifts */
#define CM_CLKSEL_DPLL_M_MASK           (0xff00)
#define CM_CLKSEL_DPLL_M_SHIFT          (8)
#define CM_CLKSEL_DPLL_N_MASK           (0x7f)
#define CM_CLKSEL_DPLL_N_SHIFT          (0)

/* HSDIVIDER (M3, M4, etc) mask */
#define CM_HSDIVIDER_CLKOUT_DIV_MASK    (0x1f)

/* Master Oscillator (Hz) */
#define CLK_M_OSC     (CONFIG_AM335X_REFERENCE_CLOCK)

/*
 * Initialize PLLs to their default values.
 */
void am335x_clock_config(void);

#endif
