/*
 * Copyright (C) 2013 F4OS Authors
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

#ifndef ARCH_CHIP_INCLUDE_TIMER_H
#define ARCH_CHIP_INCLUDE_TIMER_H

#include <stdint.h>
#include <arch/chip/registers.h>

/* Warning: Not all registers are defined for all timers! */
struct stm32f4_timer_regs {
    uint32_t CR1;   /* TIMx Control Register 1 */
    uint32_t CR2;   /* TIMx Control Register 2 */
    uint32_t SMCR;  /* TIMx Slave Mode Control Register */
    uint32_t DIER;  /* TIMx DMA/Interrupt Enable Register */
    uint32_t SR;    /* TIMx Status Register */
    uint32_t EGR;   /* TIMx Event Generation Register */
    uint32_t CCMR1; /* TIMx Capture/Compare Mode Register 1 */
    uint32_t CCMR2; /* TIMx Capture/Compare Mode Register 2 */
    uint32_t CCER;  /* TIMx Capture/Compare Enable Register */
    uint32_t CNT;   /* TIMx Counter */
    uint32_t PSC;   /* TIMx Prescaler */
    uint32_t ARR;   /* TIMx Auto-reload Register */
    uint32_t RCR;   /* TIMx Repitition Counter Register */
    uint32_t CCR1;  /* TIMx Capture/Compare Register 1 */
    uint32_t CCR2;  /* TIMx Capture/Compare Register 2 */
    uint32_t CCR3;  /* TIMx Capture/Compare Register 3 */
    uint32_t CCR4;  /* TIMx Capture/Compare Register 4 */
    uint32_t BDTR;  /* TIMx Break and Dead-time Register */
    uint32_t DCR;   /* TIMx DMA Control Register */
    uint32_t DMAR;  /* TIMx DMA Address for Full Transer */
    uint32_t OR;    /* TIMx Option Register */
};

static inline struct stm32f4_timer_regs *timer_get_regs(int num) {
    switch (num) {
        case 1: return (struct stm32f4_timer_regs *) TIM1_BASE;
        case 2: return (struct stm32f4_timer_regs *) TIM2_BASE;
        case 3: return (struct stm32f4_timer_regs *) TIM3_BASE;
        case 4: return (struct stm32f4_timer_regs *) TIM4_BASE;
        case 5: return (struct stm32f4_timer_regs *) TIM5_BASE;
        case 6: return (struct stm32f4_timer_regs *) TIM6_BASE;
        case 7: return (struct stm32f4_timer_regs *) TIM7_BASE;
        case 8: return (struct stm32f4_timer_regs *) TIM8_BASE;
        case 9: return (struct stm32f4_timer_regs *) TIM9_BASE;
        case 10: return (struct stm32f4_timer_regs *) TIM10_BASE;
        case 11: return (struct stm32f4_timer_regs *) TIM11_BASE;
        case 12: return (struct stm32f4_timer_regs *) TIM12_BASE;
        case 13: return (struct stm32f4_timer_regs *) TIM13_BASE;
        case 14: return (struct stm32f4_timer_regs *) TIM14_BASE;
    }
    return (struct stm32f4_timer_regs *) INVALID_PERIPH_BASE;
}


#define TIM_CR1_CEN         ((uint32_t) (1 << 0))   /* TIM counter enable */
#define TIM_CR1_UDIS        ((uint32_t) (1 << 1))   /* TIM update disable */
#define TIM_CR1_URS         ((uint32_t) (1 << 2))   /* TIM update request source */
#define TIM_CR1_OPM         ((uint32_t) (1 << 3))   /* TIM one-pulse mode */
#define TIM_CR1_DIR_DOWN    ((uint32_t) (1 << 4))   /* TIM downcounter */
#define TIM_CR1_CMS_EDGE    ((uint32_t) (0 << 5))   /* TIM center-aligned mode selection - counter up or down depending on DIR bit */
#define TIM_CR1_CMS_CM1     ((uint32_t) (1 << 5))   /* TIM center-aligned mode selection - up and down, compare flags set down */
#define TIM_CR1_CMS_CM2     ((uint32_t) (2 << 5))   /* TIM center-aligned mode selection - up and down, compare flags set up */
#define TIM_CR1_CMS_CM3     ((uint32_t) (3 << 5))   /* TIM center-aligned mode selection - up and down, compare flags set up/down */
#define TIM_CR1_CMS_MASK    ((uint32_t) (3 << 5))   /* TIM center-aligned mode selection mask */
#define TIM_CR1_ARPE        ((uint32_t) (1 << 7))   /* TIM auto-reload preload enable */
#define TIM_CR1_CKD_1       ((uint32_t) (0 << 8))   /* TIM clock division 1 */
#define TIM_CR1_CKD_2       ((uint32_t) (1 << 8))   /* TIM clock division 2 */
#define TIM_CR1_CKD_4       ((uint32_t) (2 << 8))   /* TIM clock division 4 */

#define TIM_CR2_CCDS        ((uint32_t) (1 << 3))   /* TIM capture/compare DMA requests send when update event occurs */
#define TIM_CR2_MMS_RST     ((uint32_t) (0 << 4))   /* TIM master mode - reset */
#define TIM_CR2_MMS_EN      ((uint32_t) (1 << 4))   /* TIM master mode - enable */
#define TIM_CR2_MMS_UP      ((uint32_t) (2 << 4))   /* TIM master mode - update */
#define TIM_CR2_MMS_CMP_PUL ((uint32_t) (3 << 4))   /* TIM master mode - compare pulse */
#define TIM_CR2_MMS_CMP_OC1 ((uint32_t) (4 << 4))   /* TIM master mode - compare OC1 */
#define TIM_CR2_MMS_CMP_OC2 ((uint32_t) (5 << 4))   /* TIM master mode - compare OC2 */
#define TIM_CR2_MMS_CMP_OC3 ((uint32_t) (6 << 4))   /* TIM master mode - compare OC3 */
#define TIM_CR2_MMS_CMP_OC4 ((uint32_t) (7 << 4))   /* TIM master mode - compare OC4 */
#define TIM_CR2_MMS_MASK    ((uint32_t) (7 << 4))   /* TIM master mode selection mask */
#define TIM_CR2_TI1_123     ((uint32_t) (1 << 7))   /* TIM CH1, CH2, CH3 pins connected to TI1 */

#define TIM_SMCR_SMS_DIS    ((uint32_t) (0 << 0))   /* TIM slave mode disabled */
#define TIM_SMCR_SMS_ENC1   ((uint32_t) (1 << 0))   /* TIM slave encoder mode 1 */
#define TIM_SMCR_SMS_ENC2   ((uint32_t) (2 << 0))   /* TIM slave encoder mode 2 */
#define TIM_SMCR_SMS_ENC3   ((uint32_t) (3 << 0))   /* TIM slave encoder mode 3 */
#define TIM_SMCR_SMS_RST    ((uint32_t) (4 << 0))   /* TIM slave reset mode */
#define TIM_SMCR_SMS_GATE   ((uint32_t) (5 << 0))   /* TIM slave gated mode */
#define TIM_SMCR_SMS_TRIG   ((uint32_t) (6 << 0))   /* TIM slave trigger mode */
#define TIM_SMCR_SMS_EXT    ((uint32_t) (7 << 0))   /* TIM slave external clock mode 1 */
#define TIM_SMCR_SMS_MASK   ((uint32_t) (7 << 0))   /* TIM slave mode select mask 1 */
#define TIM_SMCR_TS_ITR0    ((uint32_t) (0 << 4))   /* TIM internal trigger 0 */
#define TIM_SMCR_TS_ITR1    ((uint32_t) (1 << 4))   /* TIM internal trigger 1 */
#define TIM_SMCR_TS_ITR2    ((uint32_t) (2 << 4))   /* TIM internal trigger 2 */
#define TIM_SMCR_TS_ITR3    ((uint32_t) (3 << 4))   /* TIM internal trigger 3 */
#define TIM_SMCR_TS_TI1FED  ((uint32_t) (4 << 4))   /* TIM TI1 edge detector */
#define TIM_SMCR_TS_TI1FP1  ((uint32_t) (5 << 4))   /* TIM filtered timer input 1 */
#define TIM_SMCR_TS_TI1FP2  ((uint32_t) (6 << 4))   /* TIM filtered timer input 2 */
#define TIM_SMCR_TS_ETRF    ((uint32_t) (7 << 4))   /* TIM external trigger input */
#define TIM_SMCR_TS_MASK    ((uint32_t) (7 << 4))   /* TIM trigger selection mask */
#define TIM_SMCR_MSM        ((uint32_t) (1 << 7))   /* TIM master/slave mode */
#define TIM_SMCR_ETF(n)     ((uint32_t) (n << 8))   /* TIM external trigger filter */
#define TIM_SMCR_ETF_MASK   ((uint32_t) (0xf << 8)) /* TIM external trigger filter mask */
#define TIM_SMCR_ETPS_OFF   ((uint32_t) (0 << 12))  /* TIM external trigger prescaler off */
#define TIM_SMCR_ETPS_2     ((uint32_t) (1 << 12))  /* TIM external trigger prescaler 2 */
#define TIM_SMCR_ETPS_4     ((uint32_t) (2 << 12))  /* TIM external trigger prescaler 4 */
#define TIM_SMCR_ETPS_8     ((uint32_t) (3 << 12))  /* TIM external trigger prescaler 8 */
#define TIM_SMCR_ETPS_MASK  ((uint32_t) (3 << 12))  /* TIM external trigger prescaler mask */
#define TIM_SMCR_ECE        ((uint32_t) (1 << 14))  /* TIM external clock enable */
#define TIM_SMCR_ETP        ((uint32_t) (1 << 15))  /* TIM external trigger polarity */

#define TIM_DIER_UIE        ((uint32_t) (1 << 0))   /* TIM update interrupt enable */
#define TIM_DIER_CC1IE      ((uint32_t) (1 << 1))   /* TIM CC1 interrupt enable */
#define TIM_DIER_CC2IE      ((uint32_t) (1 << 2))   /* TIM CC2 interrupt enable */
#define TIM_DIER_CC3IE      ((uint32_t) (1 << 3))   /* TIM CC3 interrupt enable */
#define TIM_DIER_CC4IE      ((uint32_t) (1 << 4))   /* TIM CC4 interrupt enable */
#define TIM_DIER_TIE        ((uint32_t) (1 << 6))   /* TIM trigger interrupt enable */
#define TIM_DIER_UDE        ((uint32_t) (1 << 8))   /* TIM update DMA request enable */
#define TIM_DIER_CC1DE      ((uint32_t) (1 << 9))   /* TIM CC1 DMA request enable */
#define TIM_DIER_CC2DE      ((uint32_t) (1 << 10))  /* TIM CC2 DMA request enable */
#define TIM_DIER_CC3DE      ((uint32_t) (1 << 11))  /* TIM CC3 DMA request enable */
#define TIM_DIER_CC4DE      ((uint32_t) (1 << 12))  /* TIM CC4 DMA request enable */
#define TIM_DIER_TDE        ((uint32_t) (1 << 14))  /* TIM trigger DMA request enable */

#define TIM_EGR_UG          ((uint32_t) (1 << 0))   /* TIM Update generation */
#define TIM_EGR_CC1G        ((uint32_t) (1 << 1))   /* TIM Capture/Compare 1 generation */
#define TIM_EGR_CC2G        ((uint32_t) (1 << 2))   /* TIM Capture/Compare 2 generation */
#define TIM_EGR_CC3G        ((uint32_t) (1 << 3))   /* TIM Capture/Compare 3 generation */
#define TIM_EGR_CC4G        ((uint32_t) (1 << 4))   /* TIM Capture/Compare 4 generation */
#define TIM_EGR_COMG        ((uint32_t) (1 << 5))   /* TIM Capture/Compare control update generation */
#define TIM_EGR_TG          ((uint32_t) (1 << 6))   /* TIM Trigger generation */
#define TIM_EGR_BG          ((uint32_t) (1 << 7))   /* TIM Break generation */

#define TIM_CCMR1_CC1S_OUT  ((uint32_t) (0 << 0))   /* TIM CC1 configured as output */
#define TIM_CCMR1_CC1S_TI1  ((uint32_t) (1 << 0))   /* TIM CC1 configured as input, IC1 -> TI1 */
#define TIM_CCMR1_CC1S_TI2  ((uint32_t) (2 << 0))   /* TIM CC1 configured as input, IC1 -> TI2 */
#define TIM_CCMR1_CC1S_TRC  ((uint32_t) (3 << 0))   /* TIM CC1 configured as input, IC1 -> TRC */
#define TIM_CCMR1_CC1S_MASK ((uint32_t) (3 << 0))   /* TIM CC1 selection mask */
#define TIM_CCMR1_OC1FE     ((uint32_t) (1 << 2))   /* TIM Output compare 1 fast enable */
#define TIM_CCMR1_OC1PE     ((uint32_t) (1 << 3))   /* TIM Output compare 1 preload enable */
#define TIM_CCMR1_OC1M_FROZ ((uint32_t) (0 << 4))   /* TIM Output compare 1 mode frozen */
#define TIM_CCMR1_OC1M_SACT ((uint32_t) (1 << 4))   /* TIM Output compare 1 mode set to active on match */
#define TIM_CCMR1_OC1M_SINA ((uint32_t) (2 << 4))   /* TIM Output compare 1 mode set to inactive on match */
#define TIM_CCMR1_OC1M_TOG  ((uint32_t) (3 << 4))   /* TIM Output compare 1 mode toggle */
#define TIM_CCMR1_OC1M_FINA ((uint32_t) (4 << 4))   /* TIM Output compare 1 mode force inactive */
#define TIM_CCMR1_OC1M_FACR ((uint32_t) (5 << 4))   /* TIM Output compare 1 mode force active */
#define TIM_CCMR1_OC1M_PWM1 ((uint32_t) (6 << 4))   /* TIM Output compare 1 mode PWM 1 */
#define TIM_CCMR1_OC1M_PWM2 ((uint32_t) (7 << 4))   /* TIM Output compare 1 mode PWM 2 */
#define TIM_CCMR1_OC1M_MASK ((uint32_t) (7 << 4))   /* TIM Output compare 1 mode mask */
#define TIM_CCMR1_OC1CE     ((uint32_t) (1 << 7))   /* TIM Output compare 1 clear enable */
#define TIM_CCMR1_CC2S_OUT  ((uint32_t) (0 << 8))   /* TIM CC1 configured as output */
#define TIM_CCMR1_CC2S_TI1  ((uint32_t) (1 << 8))   /* TIM CC1 configured as input, IC2 -> TI1 */
#define TIM_CCMR1_CC2S_TI2  ((uint32_t) (2 << 8))   /* TIM CC1 configured as input, IC2 -> TI2 */
#define TIM_CCMR1_CC2S_TRC  ((uint32_t) (3 << 8))   /* TIM CC1 configured as input, IC2 -> TRC */
#define TIM_CCMR1_CC2S_MASK ((uint32_t) (3 << 8))   /* TIM CC1 selection mask */
#define TIM_CCMR1_OC2FE     ((uint32_t) (1 << 10))  /* TIM Output compare 1 fast enable */
#define TIM_CCMR1_OC2PE     ((uint32_t) (1 << 11))  /* TIM Output compare 1 preload enable */
#define TIM_CCMR1_OC2M_FROZ ((uint32_t) (0 << 12))  /* TIM Output compare 1 mode frozen */
#define TIM_CCMR1_OC2M_SACT ((uint32_t) (1 << 12))  /* TIM Output compare 1 mode set to active on match */
#define TIM_CCMR1_OC2M_SINA ((uint32_t) (2 << 12))  /* TIM Output compare 1 mode set to inactive on match */
#define TIM_CCMR1_OC2M_TOG  ((uint32_t) (3 << 12))  /* TIM Output compare 1 mode toggle */
#define TIM_CCMR1_OC2M_FINA ((uint32_t) (4 << 12))  /* TIM Output compare 1 mode force inactive */
#define TIM_CCMR1_OC2M_FACR ((uint32_t) (5 << 12))  /* TIM Output compare 1 mode force active */
#define TIM_CCMR1_OC2M_PWM1 ((uint32_t) (6 << 12))  /* TIM Output compare 1 mode PWM 1 */
#define TIM_CCMR1_OC2M_PWM2 ((uint32_t) (7 << 12))  /* TIM Output compare 1 mode PWM 2 */
#define TIM_CCMR1_OC2M_MASK ((uint32_t) (7 << 12))  /* TIM Output compare 1 mode mask */
#define TIM_CCMR1_OC2CE     ((uint32_t) (1 << 15))  /* TIM Output compare 1 clear enable */

#define TIM_CCMR2_CC3S_OUT  ((uint32_t) (0 << 0))   /* TIM CC1 configured as output */
#define TIM_CCMR2_CC3S_TI1  ((uint32_t) (1 << 0))   /* TIM CC1 configured as input, IC3 -> TI1 */
#define TIM_CCMR2_CC3S_TI2  ((uint32_t) (2 << 0))   /* TIM CC1 configured as input, IC3 -> TI2 */
#define TIM_CCMR2_CC3S_TRC  ((uint32_t) (3 << 0))   /* TIM CC1 configured as input, IC3 -> TRC */
#define TIM_CCMR2_CC3S_MASK ((uint32_t) (3 << 0))   /* TIM CC1 selection mask */
#define TIM_CCMR2_OC3FE     ((uint32_t) (1 << 2))   /* TIM Output compare 1 fast enable */
#define TIM_CCMR2_OC3PE     ((uint32_t) (1 << 3))   /* TIM Output compare 1 preload enable */
#define TIM_CCMR2_OC3M_FROZ ((uint32_t) (0 << 4))   /* TIM Output compare 1 mode frozen */
#define TIM_CCMR2_OC3M_SACT ((uint32_t) (1 << 4))   /* TIM Output compare 1 mode set to active on match */
#define TIM_CCMR2_OC3M_SINA ((uint32_t) (2 << 4))   /* TIM Output compare 1 mode set to inactive on match */
#define TIM_CCMR2_OC3M_TOG  ((uint32_t) (3 << 4))   /* TIM Output compare 1 mode toggle */
#define TIM_CCMR2_OC3M_FINA ((uint32_t) (4 << 4))   /* TIM Output compare 1 mode force inactive */
#define TIM_CCMR2_OC3M_FACR ((uint32_t) (5 << 4))   /* TIM Output compare 1 mode force active */
#define TIM_CCMR2_OC3M_PWM1 ((uint32_t) (6 << 4))   /* TIM Output compare 1 mode PWM 1 */
#define TIM_CCMR2_OC3M_PWM2 ((uint32_t) (7 << 4))   /* TIM Output compare 1 mode PWM 2 */
#define TIM_CCMR2_OC3M_MASK ((uint32_t) (7 << 4))   /* TIM Output compare 1 mode mask */
#define TIM_CCMR2_OC3CE     ((uint32_t) (1 << 7))   /* TIM Output compare 1 clear enable */
#define TIM_CCMR2_CC4S_OUT  ((uint32_t) (0 << 8))   /* TIM CC1 configured as output */
#define TIM_CCMR2_CC4S_TI1  ((uint32_t) (1 << 8))   /* TIM CC1 configured as input, IC4 -> TI1 */
#define TIM_CCMR2_CC4S_TI2  ((uint32_t) (2 << 8))   /* TIM CC1 configured as input, IC4 -> TI2 */
#define TIM_CCMR2_CC4S_TRC  ((uint32_t) (3 << 8))   /* TIM CC1 configured as input, IC4 -> TRC */
#define TIM_CCMR2_CC4S_MASK ((uint32_t) (3 << 8))   /* TIM CC1 selection mask */
#define TIM_CCMR2_OC4FE     ((uint32_t) (1 << 10))  /* TIM Output compare 1 fast enable */
#define TIM_CCMR2_OC4PE     ((uint32_t) (1 << 11))  /* TIM Output compare 1 preload enable */
#define TIM_CCMR2_OC4M_FROZ ((uint32_t) (0 << 12))  /* TIM Output compare 1 mode frozen */
#define TIM_CCMR2_OC4M_SACT ((uint32_t) (1 << 12))  /* TIM Output compare 1 mode set to active on match */
#define TIM_CCMR2_OC4M_SINA ((uint32_t) (2 << 12))  /* TIM Output compare 1 mode set to inactive on match */
#define TIM_CCMR2_OC4M_TOG  ((uint32_t) (3 << 12))  /* TIM Output compare 1 mode toggle */
#define TIM_CCMR2_OC4M_FINA ((uint32_t) (4 << 12))  /* TIM Output compare 1 mode force inactive */
#define TIM_CCMR2_OC4M_FACR ((uint32_t) (5 << 12))  /* TIM Output compare 1 mode force active */
#define TIM_CCMR2_OC4M_PWM1 ((uint32_t) (6 << 12))  /* TIM Output compare 1 mode PWM 1 */
#define TIM_CCMR2_OC4M_PWM2 ((uint32_t) (7 << 12))  /* TIM Output compare 1 mode PWM 2 */
#define TIM_CCMR2_OC4M_MASK ((uint32_t) (7 << 12))  /* TIM Output compare 1 mode mask */
#define TIM_CCMR2_OC4CE     ((uint32_t) (1 << 15))  /* TIM Output compare 1 clear enable */

#define TIM_CCER_CC1E       ((uint32_t) (1 << 0))   /* TIM Capture/Compare 1 output enable */
#define TIM_CCER_CC1P       ((uint32_t) (1 << 1))   /* TIM Capture/Compare 1 output polarity */
#define TIM_CCER_CC1NE      ((uint32_t) (1 << 2))   /* TIM Capture/Compare 1 complementary output enable */
#define TIM_CCER_CC1NP      ((uint32_t) (1 << 3))   /* TIM Capture/Compare 1 complementary output polarity */
#define TIM_CCER_CC2E       ((uint32_t) (1 << 4))   /* TIM Capture/Compare 2 output enable */
#define TIM_CCER_CC2P       ((uint32_t) (1 << 5))   /* TIM Capture/Compare 2 output polarity */
#define TIM_CCER_CC2NE      ((uint32_t) (1 << 6))   /* TIM Capture/Compare 2 complementary output enable */
#define TIM_CCER_CC2NP      ((uint32_t) (1 << 7))   /* TIM Capture/Compare 2 complementary output polarity */
#define TIM_CCER_CC3E       ((uint32_t) (1 << 8))   /* TIM Capture/Compare 3 output enable */
#define TIM_CCER_CC3P       ((uint32_t) (1 << 9))   /* TIM Capture/Compare 3 output polarity */
#define TIM_CCER_CC3NE      ((uint32_t) (1 << 10))  /* TIM Capture/Compare 3 complementary output enable */
#define TIM_CCER_CC3NP      ((uint32_t) (1 << 11))  /* TIM Capture/Compare 3 complementary output polarity */
#define TIM_CCER_CC4E       ((uint32_t) (1 << 12))  /* TIM Capture/Compare 4 output enable */
#define TIM_CCER_CC4P       ((uint32_t) (1 << 13))  /* TIM Capture/Compare 4 output polarity */
#define TIM_CCER_CC4NE      ((uint32_t) (1 << 14))  /* TIM Capture/Compare 4 complementary output enable */
#define TIM_CCER_CC4NP      ((uint32_t) (1 << 15))  /* TIM Capture/Compare 4 complementary output polarity */

#define TIM_BDTR_DTG_SHIFT  ((uint32_t) (0))        /* TIM Dead time generator shift */
#define TIM_BDTR_DTG_MASK   ((uint32_t) (0xff))     /* TIM Dead time generator mask */
#define TIM_BDTR_LOCK_OFF   ((uint32_t) (0 << 8))   /* TIM Lock off */
#define TIM_BDTR_LOCK_1     ((uint32_t) (1 << 8))   /* TIM Lock level 1 */
#define TIM_BDTR_LOCK_2     ((uint32_t) (2 << 8))   /* TIM Lock level 2 */
#define TIM_BDTR_LOCK_3     ((uint32_t) (3 << 8))   /* TIM Lock level 3 */
#define TIM_BDTR_LOCK_MASK  ((uint32_t) (3 << 8))   /* TIM Lock mask */
#define TIM_BDTR_OSSI       ((uint32_t) (1 << 10))  /* TIM Off-state selection for idle */
#define TIM_BDTR_OSSR       ((uint32_t) (1 << 11))  /* TIM Off-state selection for run */
#define TIM_BDTR_BKE        ((uint32_t) (1 << 12))  /* TIM Break enable */
#define TIM_BDTR_BKP        ((uint32_t) (1 << 13))  /* TIM Break polarity */
#define TIM_BDTR_AOE        ((uint32_t) (1 << 14))  /* TIM Automatic output enable */
#define TIM_BDTR_MOE        ((uint32_t) (1 << 15))  /* TIM Main output enable */

#endif
