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

#ifndef ARCH_CHIP_ADC_H_INCLUDED
#define ARCH_CHIP_ADC_H_INCLUDED

#include <stdint.h>

enum adc_num {
    ADC1 = 0,
    ADC2,
    ADC3,
    NUM_ADC,
};

#define ADC_CHANNELS    16

struct stm32f4_adc_individual_regs {
    uint32_t SR;    /* Status register */
    uint32_t CR1;   /* Control register 1 */
    uint32_t CR2;   /* Control register 2 */
    uint32_t SMPR1; /* Sample time register 1 */
    uint32_t SMPR2; /* Sample time register 2 */
    uint32_t JOFR1; /* Injected channel offset register 1 */
    uint32_t JOFR2; /* Injected channel offset register 2 */
    uint32_t JOFR3; /* Injected channel offset register 3 */
    uint32_t JOFR4; /* Injected channel offset register 4 */
    uint32_t HTR;   /* Watchdog higher threshold register */
    uint32_t LTR;   /* Watchdog lower threshold register */
    uint32_t SQR1;  /* Regular sequence register 1 */
    uint32_t SQR2;  /* Regular sequence register 2 */
    uint32_t SQR3;  /* Regular sequence register 3 */
    uint32_t JSQR;  /* Injected sequence register */
    uint32_t JDR1;  /* Injected data register 1 */
    uint32_t JDR2;  /* Injected data register 2 */
    uint32_t JDR3;  /* Injected data register 3 */
    uint32_t JDR4;  /* Injected data register 4 */
    uint32_t DR;    /* Regular data register */
    uint8_t reserved[0x50]; /* Padding before next register set */
};

struct stm32f4_adc_common_regs {
    uint32_t CSR;   /* Common status register */
    uint32_t CCR;   /* Common control register */
    uint32_t CDR;   /* Common data register */
};

struct stm32f4_adc_regs {
    struct stm32f4_adc_individual_regs adc[NUM_ADC];
    struct stm32f4_adc_common_regs common;
};

#define ADC_SR_AWD              ((uint32_t) (1 << 0))   /* ADC SR analog watchdog flag */
#define ADC_SR_EOC              ((uint32_t) (1 << 1))   /* ADC SR regular channel end of conversion */
#define ADC_SR_JEOC             ((uint32_t) (1 << 2))   /* ADC SR injected channel end of conversion */
#define ADC_SR_JSTRT            ((uint32_t) (1 << 3))   /* ADC SR injected channel start flag */
#define ADC_SR_STRT             ((uint32_t) (1 << 4))   /* ADC SR regular channel start flag */
#define ADC_SR_OVR              ((uint32_t) (1 << 5))   /* ADC SR overrun flag */

#define ADC_CR1_AWDCH(n)        ((uint32_t) (n))        /* ADC CR1 analog watchdog channel select */
#define ADC_CR1_AWDCH_MASK      ((uint32_t) 0x1f)       /* ADC CR1 analog watchdog channel select mask */
#define ADC_CR1_EOCIE           ((uint32_t) (1 << 5))   /* ADC CR1 interrupt enable for EOC */
#define ADC_CR1_AWDIE           ((uint32_t) (1 << 6))   /* ADC CR1 analog watchdog interrupt enable */
#define ADC_CR1_JEOCIE          ((uint32_t) (1 << 7))   /* ADC CR1 interrupt enable for inject channels */
#define ADC_CR1_SCAN            ((uint32_t) (1 << 8))   /* ADC CR1 scan mode enable */
#define ADC_CR1_AWDSGL          ((uint32_t) (1 << 9))   /* ADC CR1 analog watchdog on single channel */
#define ADC_CR1_JAUTO           ((uint32_t) (1 << 10))  /* ADC CR1 automatic injected group conversion */
#define ADC_CR1_DISCEN          ((uint32_t) (1 << 11))  /* ADC CR1 discontinuous mode on regular channels enable */
#define ADC_CR1_JDISCEN         ((uint32_t) (1 << 12))  /* ADC CR1 discontinuous mode on injected channels enable */
#define ADC_CR1_DISCNUM(n)      ((uint32_t) ((n) << 13))/* ADC CR1 discontinuous mode channel count */
#define ADC_CR1_DISCNUM_MASK    ((uint32_t) (7 << 13))  /* ADC CR1 discontinuous mode channel count */
#define ADC_CR1_JAWDEN          ((uint32_t) (1 << 22))  /* ADC CR1 analog watchdog on injected channels enable */
#define ADC_CR1_AWDEN           ((uint32_t) (1 << 23))  /* ADC CR1 analog watchdog on regular channels enable */
#define ADC_CR1_RES_12B         ((uint32_t) (0 << 24))  /* ADC CR1 12-bit resolution */
#define ADC_CR1_RES_10B         ((uint32_t) (1 << 24))  /* ADC CR1 10-bit resolution */
#define ADC_CR1_RES_8B          ((uint32_t) (2 << 24))  /* ADC CR1 8-bit resolution */
#define ADC_CR1_RES_6B          ((uint32_t) (3 << 24))  /* ADC CR1 6-bit resolution */
#define ADC_CR1_RES_MASK        ((uint32_t) (3 << 24))  /* ADC CR1 resolution mask */
#define ADC_CR1_OVRIE           ((uint32_t) (1 << 26))  /* ADC CR1 overrun interrupt enable */

#define ADC_CR2_ADON            ((uint32_t) (1 << 0))   /* ADC CR2 ADC on */
#define ADC_CR2_CONT            ((uint32_t) (1 << 1))   /* ADC CR2 continuous conversion mode */
#define ADC_CR2_DMA             ((uint32_t) (1 << 8))   /* ADC CR2 DMA mode enable */
#define ADC_CR2_DDS             ((uint32_t) (1 << 9))   /* ADC CR2 DMA disable selection */
#define ADC_CR2_EOCS            ((uint32_t) (1 << 10))  /* ADC CR2 end of conversion selection */
#define ADC_CR2_ALIGN           ((uint32_t) (1 << 11))  /* ADC CR2 data alignment */
#define ADC_CR2_JEXTSEL(n)      ((uint32_t) ((n) << 16))/* ADC CR2 external event select for injected group */
#define ADC_CR2_JEXTSEL_MASK    ((uint32_t) (0xf << 16))/* ADC CR2 external event select for injected group mask */
#define ADC_CR2_JEXTEN_DIS      ((uint32_t) (0 << 20))  /* ADC CR2 external trigger disable for injected channels */
#define ADC_CR2_JEXTEN_RISE     ((uint32_t) (1 << 20))  /* ADC CR2 external trigger on rising edge for injected channels */
#define ADC_CR2_JEXTEN_FALL     ((uint32_t) (2 << 20))  /* ADC CR2 external trigger on falling edge for injected channels */
#define ADC_CR2_JEXTEN_BOTH     ((uint32_t) (3 << 20))  /* ADC CR2 external trigger on rising and falling edges for injected channels */
#define ADC_CR2_JEXTEN_MASK     ((uint32_t) (3 << 20))  /* ADC CR2 external trigger for injected channels mask */
#define ADC_CR2_JSWSTART        ((uint32_t) (1 << 22))  /* ADC CR2 start conversion of injected channels */
#define ADC_CR2_EXTSEL(n)       ((uint32_t) ((n) << 24))/* ADC CR2 external event select for regular group */
#define ADC_CR2_EXTSEL_MASK     ((uint32_t) (0xf << 24))/* ADC CR2 external event select for regular group mask */
#define ADC_CR2_EXTEN_DIS       ((uint32_t) (0 << 28))  /* ADC CR2 external trigger disable for regular channels */
#define ADC_CR2_EXTEN_RISE      ((uint32_t) (1 << 28))  /* ADC CR2 external trigger on rising edge for regular channels */
#define ADC_CR2_EXTEN_FALL      ((uint32_t) (2 << 28))  /* ADC CR2 external trigger on falling edge for regular channels */
#define ADC_CR2_EXTEN_BOTH      ((uint32_t) (3 << 28))  /* ADC CR2 external trigger on rising and falling edges for regular channels */
#define ADC_CR2_EXTEN_MASK      ((uint32_t) (3 << 28))  /* ADC CR2 external trigger for regular channels mask */
#define ADC_CR2_SWSTART         ((uint32_t) (1 << 30))  /* ADC CR2 start conversion of regular channels */

#define ADC_SMP_3               ((uint32_t) (0))        /* ADC 3 cycle sampling time */
#define ADC_SMP_15              ((uint32_t) (1))        /* ADC 15 cycle sampling time */
#define ADC_SMP_28              ((uint32_t) (2))        /* ADC 28 cycle sampling time */
#define ADC_SMP_56              ((uint32_t) (3))        /* ADC 56 cycle sampling time */
#define ADC_SMP_84              ((uint32_t) (4))        /* ADC 84 cycle sampling time */
#define ADC_SMP_112             ((uint32_t) (5))        /* ADC 112 cycle sampling time */
#define ADC_SMP_144             ((uint32_t) (6))        /* ADC 144 cycle sampling time */
#define ADC_SMP_480             ((uint32_t) (7))        /* ADC 480 cycle sampling time */

#define ADC_SMPR1_SMP(n, v)     ((uint32_t) ((v) << (3*((n)-10))))  /* ADC SMPR1 channel n, sample rate v */
#define ADC_SMPR1_SMP_MASK(n)   ((uint32_t) (7 << (3*((n)-10))))    /* ADC SMPR1 channel n sample rate mask */

#define ADC_SMPR2_SMP(n, v)     ((uint32_t) ((v) << (3*(n))))       /* ADC SMPR2 channel n, sample rate v */
#define ADC_SMPR2_SMP_MASK(n)   ((uint32_t) (7 << (3*(n))))         /* ADC SMPR2 channel n sample rate mask */

#define ADC_SQR1_LEN(n)         ((uint32_t) (((n)-1) << 20))        /* ADC SQR1 regular channel sequence length */
#define ADC_SQR1_LEN_MASK       ((uint32_t) (0xf << 20))            /* ADC SQR1 regular channel sequence length mask */
#define ADC_SQR1_LEN_SHIFT      ((uint32_t) (20))                   /* ADC SQR1 regular channel sequence length bit shift */
#define ADC_SQR1_SQ_SHIFT(n)    ((uint32_t) (5*((n)-13)))                   /* ADC SQR1 nth conversion in sequence bit shift */
#define ADC_SQR1_SQ(n, v)       ((uint32_t) ((v) << ADC_SQR1_SQ_SHIFT(n)))  /* ADC SQR1 nth conversion in sequence = v */
#define ADC_SQR1_SQ_MASK(n)     ((uint32_t) (0x1f << ADC_SQR1_SQ_SHIFT(n))) /* ADC SQR1 nth conversion in sequence mask */

#define ADC_SQR2_SQ_SHIFT(n)    ((uint32_t) (5*((n)-7)))                    /* ADC SQR2 nth conversion in sequence bit shift */
#define ADC_SQR2_SQ(n, v)       ((uint32_t) ((v) << ADC_SQR2_SQ_SHIFT(n)))  /* ADC SQR2 nth conversion in sequence = v */
#define ADC_SQR2_SQ_MASK(n)     ((uint32_t) (0x1f << ADC_SQR2_SQ_SHIFT(n))) /* ADC SQR2 nth conversion in sequence mask */

#define ADC_SQR3_SQ_SHIFT(n)    ((uint32_t) (5*((n)-1)))                    /* ADC SQR3 nth conversion in sequence bit shift */
#define ADC_SQR3_SQ(n, v)       ((uint32_t) ((v) << ADC_SQR3_SQ_SHIFT(n)))  /* ADC SQR3 nth conversion in sequence = v */
#define ADC_SQR3_SQ_MASK(n)     ((uint32_t) (0x1f << ADC_SQR3_SQ_SHIFT(n))) /* ADC SQR3 nth conversion in sequence mask */

#endif
