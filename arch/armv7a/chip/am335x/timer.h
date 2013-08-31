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

#ifndef ARCH_ARMV7A_CHIP_AM335X_TIMER_H_INCLUDED
#define ARCH_ARMV7A_CHIP_AM335X_TIMER_H_INCLUDED

#include <stdint.h>

struct am335x_dmtimer {
    uint32_t tidr;
    uint32_t res1[3];
    uint32_t tiocp_cfg;
    uint32_t res2[3];
    uint32_t irq_eqi;
    uint32_t irqstatus_raw;
    uint32_t irqstatus;
    uint32_t irqenable_set;
    uint32_t irqenable_clear;
    uint32_t irqwakeen;
    uint32_t tclr;
    uint32_t tcrr;
    uint32_t tldr;
    uint32_t ttgr;
    uint32_t twps;
    uint32_t tmar;
    uint32_t tcar1;
    uint32_t tsicr;
    uint32_t tcar2;
};

/* DMTIMER1 is a special 1ms timer */
struct am335x_dmtimer_1ms {
    uint32_t tidr;
    uint32_t res1[3];
    uint32_t tiocp_cfg;
    uint32_t tistat;
    uint32_t tisr;
    uint32_t tier;
    uint32_t twer;
    uint32_t tclr;
    uint32_t tcrr;
    uint32_t tldr;
    uint32_t ttgr;
    uint32_t twps;
    uint32_t tmar;
    uint32_t tcar1;
    uint32_t tsicr;
    uint32_t tcar2;
    uint32_t tpir;
    uint32_t tnir;
    uint32_t tcvr;
    uint32_t tocr;
    uint32_t towr;
};

#define AM335X_DMTIMER_TISR_MAT_IT_FLAG (1 << 0)    /* Timer match interrupt flag */
#define AM335X_DMTIMER_TISR_OVF_IT_FLAG (1 << 1)    /* Timer overflow interrupt flag */
#define AM335X_DMTIMER_TISR_TCAR_IT_FLAG (1 << 2)   /* Timer capture interrupt flag */

#define AM335X_DMTIMER_TIER_MAT_IT_EN   (1 << 0)    /* Timer match interrupt enable */
#define AM335X_DMTIMER_TIER_OVF_IT_EN   (1 << 1)    /* Timer overflow interrupt enable */
#define AM335X_DMTIMER_TIER_TCAR_IT_EN  (1 << 2)    /* Timer capture interrupt enable */

#define AM335X_DMTIMER_TCLR_ST          (1 << 0)    /* Timer start/stop */
#define AM335X_DMTIMER_TCLR_AR          (1 << 1)    /* Timer auto-reload enable */
#define AM335X_DMTIMER_TCLR_PTV_MASK    (0x7 << 2)  /* Timer trigger output mode mask */
#define AM335X_DMTIMER_TCLR_PRE         (1 << 5)    /* Timer prescaler enable */
#define AM335X_DMTIMER_TCLR_CE          (1 << 6)    /* Timer compare enable */
#define AM335X_DMTIMER_TCLR_SCPWM       (1 << 7)    /* Timer PWM output default high */
#define AM335X_DMTIMER_TCLR_TCM_MASK    (0x3 << 8)  /* Timer transition capture mode mask */
#define AM335X_DMTIMER_TCLR_TCM_NOCAP   (0)         /* Timer TCM no capture */
#define AM335X_DMTIMER_TCLR_TCM_RISE    (1)         /* Timer TCM rising edge capture */
#define AM335X_DMTIMER_TCLR_TCM_FALL    (2)         /* Timer TCM falling edge capture */
#define AM335X_DMTIMER_TCLR_TCM_BOTH    (3)         /* Timer TCM both edge capture */
#define AM335X_DMTIMER_TCLR_TRIG_MASK   (0x3 << 10) /* Timer trigger output mode mask */
#define AM335X_DMTIMER_TCLR_TRIG_NONE   (0)         /* Timer trigger no trigger */
#define AM335X_DMTIMER_TCLR_TRIG_OVR    (1)         /* Timer trigger overflow */
#define AM335X_DMTIMER_TCLR_TRIG_OVR_MAT (2)        /* Timer trigger overflow and match */
#define AM335X_DMTIMER_TCLR_PT          (1 << 12)   /* Timer Pulse/Toggle mode */
#define AM335X_DMTIMER_TCLR_CAPT_MODE   (1 << 13)   /* Timer capture mode */

#define AM335X_DMTIMER_1MS_CLK_M_OSC    (0x0)       /* DMTimer 1ms CLK_M_OSC source */
#define AM335X_DMTIMER_1MS_CLK_32KHZ    (0x1)       /* DMTimer 1ms CLK_32KHZ source */
#define AM335X_DMTIMER_1MS_TCLKIN       (0x2)       /* DMTimer 1ms TCLKIN source */
#define AM335X_DMTIMER_1MS_CLK_RC32K    (0x3)       /* DMTimer 1ms CLK_RC32K source */
#define AM335X_DMTIMER_1MS_CLK_32768    (0x4)       /* DMTimer 1ms CLK_32768 source */

#endif
