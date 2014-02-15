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

/*
 * STM32F4 PWM peripheral using timers 1, 3, 4, 8, 9, 10, 11, 12, 13, 14.
 * Timers 2 and 5 are used for the perfcounter, and 6 and 7 do not support
 * PWM outputs.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arch/chip/gpio.h>
#include <arch/chip/registers.h>
#include <arch/chip/timer.h>
#include <dev/hw/gpio.h>
#include <dev/hw/pwm.h>
#include <dev/raw_mem.h>
#include <kernel/semaphore.h>
#include <mm/mm.h>

struct stm32f4_pwm {
    uint32_t timer_channel;
    uint8_t timer;
    uint8_t channel;
};

/* Timers used by PWM peripheral */
enum timers {
    TIM1,
    TIM3,
    TIM4,
    TIM8,
    TIM9,
    TIM10,
    TIM11,
    TIM12,
    TIM13,
    TIM14,
    PWM_NUM_TIMERS,
};

/* Timer channels available for use */
enum timer_channels {
    INVALID_TIMER_CHANNEL = 0,
    TIM1_CH1 = (1 << 0),
    TIM1_CH2 = (1 << 1),
    TIM1_CH3 = (1 << 2),
    TIM1_CH4 = (1 << 3),
    TIM3_CH1 = (1 << 4),
    TIM3_CH2 = (1 << 5),
    TIM3_CH3 = (1 << 6),
    TIM3_CH4 = (1 << 7),
    TIM4_CH1 = (1 << 8),
    TIM4_CH2 = (1 << 9),
    TIM4_CH3 = (1 << 10),
    TIM4_CH4 = (1 << 11),
    TIM8_CH1 = (1 << 12),
    TIM8_CH2 = (1 << 13),
    TIM8_CH3 = (1 << 14),
    TIM8_CH4 = (1 << 15),
    TIM9_CH1 = (1 << 16),
    TIM9_CH2 = (1 << 17),
    TIM10_CH1 = (1 << 18),
    TIM10_CH2 = (1 << 19),
    TIM11_CH1 = (1 << 20),
    TIM11_CH2 = (1 << 21),
    TIM12_CH1 = (1 << 22),
    TIM12_CH2 = (1 << 23),
    TIM13_CH1 = (1 << 24),
    TIM13_CH2 = (1 << 25),
    TIM14_CH1 = (1 << 26),
    TIM14_CH2 = (1 << 27),
    PWM_NUM_CHANNELS = 28,
};

/*
 * Array of bitfields mapping GPIOs to valid output timer channels.
 * At the GPIO's index into the array, values from enum timer_channels
 * are ORed together, representing all of the timer channels that can be
 * used as a PWM output from that GPIO.
 *
 * Timers 2 and 5 are unavailable, and inverted outputs are unused.
 */
static uint32_t gpio_to_timer_channel[STM32F4_NUM_GPIOS] = {
    [0 ... (STM32F4_NUM_GPIOS-1)] = 0,  /* Default to no channels */
    [STM32F4_GPIO_PA0] = 0 /* TIM2_CH1, TIM5_CH1 */,
    [STM32F4_GPIO_PA1] = 0 /* TIM2_CH2, TIM5_CH2 */,
    [STM32F4_GPIO_PA2] = TIM9_CH1 /* TIM2_CH3, TIM5_CH3 */,
    [STM32F4_GPIO_PA3] = TIM9_CH2 /* TIM2_CH3, TIM5_CH3 */,
    [STM32F4_GPIO_PA5] = 0 /* TIM2_CH1, TIM8_CH1N */,
    [STM32F4_GPIO_PA6] = TIM3_CH1 | TIM13_CH1,
    [STM32F4_GPIO_PA7] = TIM3_CH2 | TIM14_CH1 /* TIM1_CH1N, TIM8_CH1N */,
    [STM32F4_GPIO_PA8] = TIM1_CH1,
    [STM32F4_GPIO_PA9] = TIM1_CH2,
    [STM32F4_GPIO_PA10] = TIM1_CH3,
    [STM32F4_GPIO_PA11] = TIM1_CH4,
    [STM32F4_GPIO_PA15] = 0 /* TIM2_CH1 */,
    [STM32F4_GPIO_PB0] = TIM3_CH3 /* TIM1_CH2N, TIM8_CH2N */,
    [STM32F4_GPIO_PB1] = TIM3_CH4 /* TIM1_CH3N, TIM8_CH3N */,
    [STM32F4_GPIO_PB3] = 0 /* TIM2_CH2 */,
    [STM32F4_GPIO_PB4] = TIM3_CH1,
    [STM32F4_GPIO_PB5] = TIM3_CH2,
    [STM32F4_GPIO_PB6] = TIM4_CH1,
    [STM32F4_GPIO_PB7] = TIM4_CH2,
    [STM32F4_GPIO_PB8] = TIM4_CH3 | TIM10_CH1,
    [STM32F4_GPIO_PB9] = TIM4_CH4 | TIM11_CH1,
    [STM32F4_GPIO_PB10] = 0 /* TIM2_CH3 */,
    [STM32F4_GPIO_PB11] = 0 /* TIM2_CH4 */,
    [STM32F4_GPIO_PB13] = 0 /* TIM1_CH1N */,
    [STM32F4_GPIO_PB14] = TIM12_CH1 /* TIM1_CH2N, TIM8_CH2N */,
    [STM32F4_GPIO_PB15] = TIM12_CH2 /* TIM1_CH3N, TIM8_CH3N */,
    [STM32F4_GPIO_PC6] = TIM3_CH1 | TIM8_CH1,
    [STM32F4_GPIO_PC7] = TIM3_CH2 | TIM8_CH2,
    [STM32F4_GPIO_PC8] = TIM3_CH3 | TIM8_CH3,
    [STM32F4_GPIO_PC9] = TIM3_CH4 | TIM8_CH4,
    [STM32F4_GPIO_PD12] = TIM4_CH1,
    [STM32F4_GPIO_PD13] = TIM4_CH2,
    [STM32F4_GPIO_PD14] = TIM4_CH3,
    [STM32F4_GPIO_PD15] = TIM4_CH4,
    [STM32F4_GPIO_PE5] = TIM9_CH1,
    [STM32F4_GPIO_PE6] = TIM9_CH2,
    [STM32F4_GPIO_PE8] = 0 /* TIM1_CH1N */,
    [STM32F4_GPIO_PE9] = TIM1_CH1,
    [STM32F4_GPIO_PE10] = 0 /* TIM1_CH2N */,
    [STM32F4_GPIO_PE11] = TIM1_CH2,
    [STM32F4_GPIO_PE12] = 0 /* TIM1_CH3N */,
    [STM32F4_GPIO_PE13] = TIM1_CH3,
    [STM32F4_GPIO_PE14] = TIM1_CH4,
    [STM32F4_GPIO_PF6] = TIM10_CH1,
    [STM32F4_GPIO_PF7] = TIM11_CH1,
    [STM32F4_GPIO_PF8] = TIM13_CH1,
    [STM32F4_GPIO_PF9] = TIM14_CH1,
    [STM32F4_GPIO_PH6] = TIM12_CH1,
    [STM32F4_GPIO_PH9] = TIM12_CH2,
    [STM32F4_GPIO_PH10] = 0 /* TIM5_CH1 */,
    [STM32F4_GPIO_PH11] = 0 /* TIM5_CH2 */,
    [STM32F4_GPIO_PH12] = 0 /* TIM5_CH3 */,
    [STM32F4_GPIO_PH13] = 0 /* TIM8_CH1N */,
    [STM32F4_GPIO_PH14] = 0 /* TIM8_CH2N */,
    [STM32F4_GPIO_PH15] = 0 /* TIM8_CH3N */,
    [STM32F4_GPIO_PI0] = 0 /* TIM5_CH4 */,
    [STM32F4_GPIO_PI2] = TIM8_CH4,
    [STM32F4_GPIO_PI5] = TIM8_CH1,
    [STM32F4_GPIO_PI6] = TIM8_CH2,
    [STM32F4_GPIO_PI7] = TIM8_CH3,
};

/*
 * Bitfield of enum timer_channel values, indicating which
 * timer channels are not in use.  0 bits indicate in use.
 */
static uint32_t channels_available = 0xffffffff;

/* Lock for channel_available */
static struct semaphore channels_available_sem = INIT_SEMAPHORE;

/* Lock for each timer */
static struct semaphore timer_sem[PWM_NUM_TIMERS] = {
    [0 ... (PWM_NUM_TIMERS-1)] = INIT_SEMAPHORE,
};

/* Get the timer number for a given timer channel */
static uint8_t timer_channel_to_timer(enum timer_channels channel) {
    switch (channel) {
        case TIM1_CH1:
        case TIM1_CH2:
        case TIM1_CH3:
        case TIM1_CH4:
            return 1;
        case TIM3_CH1:
        case TIM3_CH2:
        case TIM3_CH3:
        case TIM3_CH4:
            return 3;
        case TIM4_CH1:
        case TIM4_CH2:
        case TIM4_CH3:
        case TIM4_CH4:
            return 4;
        case TIM8_CH1:
        case TIM8_CH2:
        case TIM8_CH3:
        case TIM8_CH4:
            return 8;
        case TIM9_CH1:
        case TIM9_CH2:
            return 9;
        case TIM10_CH1:
        case TIM10_CH2:
            return 10;
        case TIM11_CH1:
        case TIM11_CH2:
            return 11;
        case TIM12_CH1:
        case TIM12_CH2:
            return 12;
        case TIM13_CH1:
        case TIM13_CH2:
            return 13;
        case TIM14_CH1:
        case TIM14_CH2:
            return 14;
        default:
            return 0;
    }
}

/* Get the channel number for a given timer channel, without timer info */
static uint8_t timer_channel_to_channel(enum timer_channels channel) {
    switch (channel) {
        case TIM1_CH1:
        case TIM3_CH1:
        case TIM4_CH1:
        case TIM8_CH1:
        case TIM9_CH1:
        case TIM10_CH1:
        case TIM11_CH1:
        case TIM12_CH1:
        case TIM13_CH1:
        case TIM14_CH1:
            return 1;
        case TIM1_CH2:
        case TIM3_CH2:
        case TIM4_CH2:
        case TIM8_CH2:
        case TIM9_CH2:
        case TIM10_CH2:
        case TIM11_CH2:
        case TIM12_CH2:
        case TIM13_CH2:
        case TIM14_CH2:
            return 2;
        case TIM1_CH3:
        case TIM3_CH3:
        case TIM4_CH3:
        case TIM8_CH3:
            return 3;
        case TIM1_CH4:
        case TIM3_CH4:
        case TIM4_CH4:
        case TIM8_CH4:
            return 4;
        default:
            return 0;
    }
}

/*
 * Bit field of all channels in timer
 *
 * @param timer Timer number to get channels from
 * @return Bitfield of enum timer_channel values of all channels in timer
 */
static uint32_t timer_all_channels(uint8_t timer) {
    switch (timer) {
        case 1: return TIM1_CH1 | TIM1_CH2 | TIM1_CH3 | TIM1_CH4;
        case 3: return TIM3_CH1 | TIM3_CH2 | TIM3_CH3 | TIM3_CH4;
        case 4: return TIM4_CH1 | TIM4_CH2 | TIM4_CH3 | TIM4_CH4;
        case 8: return TIM8_CH1 | TIM8_CH2 | TIM8_CH3 | TIM8_CH4;
        case 9: return TIM9_CH1 | TIM9_CH2;
        case 10: return TIM10_CH1 | TIM10_CH2;
        case 11: return TIM11_CH1 | TIM11_CH2;
        case 12: return TIM12_CH1 | TIM12_CH2;
        case 13: return TIM13_CH1 | TIM13_CH2;
        case 14: return TIM14_CH1 | TIM14_CH2;
        default: return 0;
    }
}

/*
 * Get the timer_sem semaphore for the given timer number.
 * The timer number cannot be used to directly index into the array,
 * as not all timers are used.
 */
static struct semaphore *timer_semaphore(uint8_t timer) {
    switch (timer) {
        case 1: return &timer_sem[TIM1];
        case 3: return &timer_sem[TIM3];
        case 4: return &timer_sem[TIM4];
        case 8: return &timer_sem[TIM8];
        case 9 : return &timer_sem[TIM9];
        case 10: return &timer_sem[TIM10];
        case 11: return &timer_sem[TIM11];
        case 12: return &timer_sem[TIM12];
        case 13: return &timer_sem[TIM13];
        case 14: return &timer_sem[TIM14];
        default: return NULL;
    }
}

/*
 * Determine timer master clock
 *
 * Timers are clocked at 2 * APB clock.
 * APB1 is system clock / 4, APB2 is system clock / 2.
 *
 * @param timer Timer to get master clock of
 * @return master clock speed in hertz
 */
uint32_t timer_clock(uint8_t timer) {
    switch (timer) {
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 12:
        case 13:
        case 14:
            return CONFIG_SYS_CLOCK/2;
        case 1:
        case 8:
        case 9:
        case 10:
        case 11:
            return CONFIG_SYS_CLOCK;
        default:
            return 0;
    }
}

/* Get timer GPIO alternative function */
int timer_gpio_af(uint8_t timer) {
    switch (timer) {
        case 1: return STM32F4_GPIO_AF_TIM1;
        case 2: return STM32F4_GPIO_AF_TIM2;
        case 3: return STM32F4_GPIO_AF_TIM3;
        case 4: return STM32F4_GPIO_AF_TIM4;
        case 5: return STM32F4_GPIO_AF_TIM5;
        case 8: return STM32F4_GPIO_AF_TIM8;
        case 9: return STM32F4_GPIO_AF_TIM9;
        case 10: return STM32F4_GPIO_AF_TIM10;
        case 11: return STM32F4_GPIO_AF_TIM11;
        case 12: return STM32F4_GPIO_AF_TIM12;
        case 13: return STM32F4_GPIO_AF_TIM13;
        case 14: return STM32F4_GPIO_AF_TIM14;
        default: return -1;
    }
}

/* Return capture/compare register for given timer */
uint32_t *timer_ccr_register(uint8_t timer, uint8_t channel) {
    struct stm32f4_timer_regs *regs = timer_get_regs(timer);

    switch (channel) {
        case 1: return &regs->CCR1;
        case 2: return &regs->CCR2;
        case 3: return &regs->CCR3;
        case 4: return &regs->CCR4;
        default: return NULL;
    }
}

/*
 * Determine if timer is enabled
 *
 * Timer considered enabled if "Counter enable" bit is set
 *
 * @param timer Timer number
 * @return 1 if timer enabled, 0 if not
 */
static int timer_enabled(uint8_t timer) {
    struct stm32f4_timer_regs *regs = timer_get_regs(timer);

    return raw_mem_read(&regs->CR1) & TIM_CR1_CEN;
}

/*
 * Determine timer duty cycle, in microseconds
 *
 * Computes the duty cycle of the timer in its current configuration.
 * Only upcounting mode is supported, others will result in an error.
 *
 * @param timer Timer to get duty cycle of
 * @returns duty cycle in microseconds, negative on error
 */
static int timer_duty_cycle(uint8_t timer) {
    struct stm32f4_timer_regs *regs = timer_get_regs(timer);
    uint32_t cr1 = raw_mem_read(&regs->CR1);
    uint32_t prescaler = raw_mem_read(&regs->PSC) + 1;
    uint32_t reload = raw_mem_read(&regs->ARR);
    uint32_t clock = timer_clock(timer);
    uint32_t frequency = clock/prescaler;
    uint32_t duty_cycle = (1e6*reload)/frequency;

    /* Ensure edge-aligned mode */
    if ((cr1 & TIM_CR1_CMS_MASK) != TIM_CR1_CMS_EDGE) {
        return -1;
    }

    /* Ensure upcounting mode */
    if (cr1 & TIM_CR1_DIR_DOWN) {
        return -1;
    }

    return duty_cycle;
}

/*
 * Initialize and enable timer
 *
 * Set the duty cycle and begin counting
 *
 * @param timer Timer to initialize
 * @param duty  Duty cycle, in microseconds, to configure
 * @returns zero on success, negative on error
 */
int timer_initialize(uint8_t timer, uint32_t duty) {
    struct stm32f4_timer_regs *regs = timer_get_regs(timer);
    uint32_t clock = timer_clock(timer);

    /* Enable timer clock */
    /* FIXME: move this to some RCC file */
    switch (timer) {
    case 1:
        *RCC_APB2ENR |= RCC_APB2ENR_TIM1EN;
        break;
    case 2:
        *RCC_APB1ENR |= RCC_APB1ENR_TIM2EN;
        break;
    case 3:
        *RCC_APB1ENR |= RCC_APB1ENR_TIM3EN;
        break;
    case 4:
        *RCC_APB1ENR |= RCC_APB1ENR_TIM4EN;
        break;
    case 5:
        *RCC_APB1ENR |= RCC_APB1ENR_TIM5EN;
        break;
    case 6:
        *RCC_APB1ENR |= RCC_APB1ENR_TIM6EN;
        break;
    case 7:
        *RCC_APB1ENR |= RCC_APB1ENR_TIM7EN;
        break;
    case 8:
        *RCC_APB2ENR |= RCC_APB2ENR_TIM8EN;
        break;
    case 9:
        *RCC_APB2ENR |= RCC_APB2ENR_TIM9EN;
        break;
    case 10:
        *RCC_APB2ENR |= RCC_APB2ENR_TIM10EN;
        break;
    case 11:
        *RCC_APB2ENR |= RCC_APB2ENR_TIM11EN;
        break;
    case 12:
        *RCC_APB1ENR |= RCC_APB1ENR_TIM12EN;
        break;
    case 13:
        *RCC_APB1ENR |= RCC_APB1ENR_TIM13EN;
        break;
    case 14:
        *RCC_APB1ENR |= RCC_APB1ENR_TIM14EN;
        break;
    }

    /* Prescale to 1MHz clock */
    raw_mem_write(&regs->PSC, clock/1e6 - 1);

    /* Each cycle is 1us, reload to desired duty cycle */
    raw_mem_write(&regs->ARR, duty);

    /* Update event every overflow */
    raw_mem_write(&regs->RCR, 0);

    /* Upcounting mode */
    raw_mem_clear_bits(&regs->CR1, TIM_CR1_DIR_DOWN);

    /* Edge-aligned mode */
    raw_mem_set_mask(&regs->CR1, TIM_CR1_CMS_MASK, TIM_CR1_CMS_EDGE);

    /* Disable outputs */
    raw_mem_clear_bits(&regs->CCER, TIM_CCER_CC1E | TIM_CCER_CC2E);
    if (timer == 1 || timer == 8) {
        raw_mem_clear_bits(&regs->CCER, TIM_CCER_CC3E | TIM_CCER_CC4E);

        /* Main output enable */
        raw_mem_set_bits(&regs->BDTR, TIM_BDTR_MOE);
    }

    /* Enable timer and ARR preload */
    raw_mem_set_bits(&regs->CR1, TIM_CR1_CEN | TIM_CR1_ARPE);

    /* Generate update event to load ARR and begin counting */
    raw_mem_set_bits(&regs->EGR, TIM_EGR_UG);

    return 0;
}

/*
 * Configure channel as PWM output
 *
 * Prepare timer channel to be a PWM output, but do not enable it
 *
 * @param timer Timer number
 * @param channel   Channel number in timer
 * @return 0 on success, negative on error
 */
int timer_configure_channel(uint8_t timer, uint8_t channel) {
    struct stm32f4_timer_regs *regs = timer_get_regs(timer);

    /*
     * Set all capture/compare mode bits:
     * CC configured as output
     * OC fast enable disabled
     * OC preload enabled
     * OC PWM 1 mode
     * OC clear enable disabled
     */

    switch (channel) {
        case 1:
            raw_mem_set_mask(&regs->CCMR1,
                    TIM_CCMR1_CC1S_MASK | TIM_CCMR1_OC1FE | TIM_CCMR1_OC1PE |
                    TIM_CCMR1_OC1M_MASK | TIM_CCMR1_OC1CE,
                    TIM_CCMR1_CC1S_OUT | TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_PWM1);
            break;
        case 2:
            raw_mem_set_mask(&regs->CCMR1,
                    TIM_CCMR1_CC2S_MASK | TIM_CCMR1_OC2FE | TIM_CCMR1_OC2PE |
                    TIM_CCMR1_OC2M_MASK | TIM_CCMR1_OC2CE,
                    TIM_CCMR1_CC2S_OUT | TIM_CCMR1_OC2PE | TIM_CCMR1_OC2M_PWM1);
            break;
        case 3:
            raw_mem_set_mask(&regs->CCMR2,
                    TIM_CCMR2_CC3S_MASK | TIM_CCMR2_OC3FE | TIM_CCMR2_OC3PE |
                    TIM_CCMR2_OC3M_MASK | TIM_CCMR2_OC3CE,
                    TIM_CCMR2_CC3S_OUT | TIM_CCMR2_OC3PE | TIM_CCMR2_OC3M_PWM1);
            break;
        case 4:
            raw_mem_set_mask(&regs->CCMR2,
                    TIM_CCMR2_CC4S_MASK | TIM_CCMR2_OC4FE | TIM_CCMR2_OC4PE |
                    TIM_CCMR2_OC4M_MASK | TIM_CCMR2_OC4CE,
                    TIM_CCMR2_CC4S_OUT | TIM_CCMR2_OC4PE | TIM_CCMR2_OC4M_PWM1);
            break;
        default:
            return -1;
    }

    return 0;
}

static int stm32f4_pwm_enable(struct pwm *pwm, uint8_t enable) {
    struct stm32f4_pwm *stm32_pwm = pwm->priv;
    struct stm32f4_timer_regs *regs = timer_get_regs(stm32_pwm->timer);
    uint32_t reg;

    switch (stm32_pwm->channel) {
        case 1:
            reg = TIM_CCER_CC1E;
            break;
        case 2:
            reg = TIM_CCER_CC2E;
            break;
        case 3:
            reg = TIM_CCER_CC3E;
            break;
        case 4:
            reg = TIM_CCER_CC4E;
            break;
        default:
            return -1;
    }

    acquire(timer_semaphore(stm32_pwm->timer));

    if (enable) {
        raw_mem_set_bits(&regs->CCER, reg);
    }
    else {
        raw_mem_clear_bits(&regs->CCER, reg);
    }

    release(timer_semaphore(stm32_pwm->timer));

    return 0;
}

static int32_t stm32f4_pwm_set_duty_cycle(struct pwm *pwm, uint32_t duty) {
    struct stm32f4_pwm *stm32_pwm = pwm->priv;
    struct stm32f4_timer_regs *regs = timer_get_regs(stm32_pwm->timer);
    uint32_t clock = timer_clock(stm32_pwm->timer);
    uint32_t all_channels = timer_all_channels(stm32_pwm->timer);
    uint32_t timer_channels;

    /* Verify that this is the only channel in use in the timer */
    acquire(&channels_available_sem);

    timer_channels = channels_available & all_channels;

    if ((~stm32_pwm->timer_channel & all_channels) != timer_channels) {
        /* Other channels on this timer in use */
        goto err;
    }

    acquire(timer_semaphore(stm32_pwm->timer));

    /* Prescale to 1MHz clock */
    raw_mem_write(&regs->PSC, clock/1e6 - 1);

    /* Each cycle is 1us, reload to desired duty cycle */
    raw_mem_write(&regs->ARR, duty);

    /* Generate update event to reload ARR */
    raw_mem_set_bits(&regs->EGR, TIM_EGR_UG);

    release(timer_semaphore(stm32_pwm->timer));

    release(&channels_available_sem);

    return duty;

err:
    release(&channels_available_sem);
    return -1;
}

static uint32_t stm32f4_pwm_get_duty_cycle(struct pwm *pwm) {
    struct stm32f4_pwm *stm32_pwm = pwm->priv;

    return timer_duty_cycle(stm32_pwm->timer);
}

static int32_t stm32f4_pwm_set_pulse_width(struct pwm *pwm, uint32_t width) {
    struct stm32f4_pwm *stm32_pwm = pwm->priv;
    struct stm32f4_timer_regs *regs = timer_get_regs(stm32_pwm->timer);
    uint32_t clock = timer_clock(stm32_pwm->timer);
    uint32_t prescaler = raw_mem_read(&regs->PSC) + 1;
    uint32_t frequency = clock/prescaler;
    uint32_t value = width*(frequency/1e6);
    uint32_t *ccr = timer_ccr_register(stm32_pwm->timer, stm32_pwm->channel);
    if (!ccr) {
        return -1;
    }

    acquire(timer_semaphore(stm32_pwm->timer));
    raw_mem_write(ccr, value);
    release(timer_semaphore(stm32_pwm->timer));

    /*
     * Value will be loaded into preload register and begin outputting at next
     * update event (overflow).
     */

    return value;
}

static uint32_t stm32f4_pwm_get_pulse_width(struct pwm *pwm) {
    struct stm32f4_pwm *stm32_pwm = pwm->priv;
    struct stm32f4_timer_regs *regs = timer_get_regs(stm32_pwm->timer);
    uint32_t value, clock = timer_clock(stm32_pwm->timer);
    uint32_t prescaler = raw_mem_read(&regs->PSC) + 1;
    uint32_t frequency = clock/prescaler;
    uint32_t *ccr = timer_ccr_register(stm32_pwm->timer, stm32_pwm->channel);
    if (!ccr) {
        return -1;
    }

    value = raw_mem_read(ccr);

    return (1e6*value)/frequency;
}

static uint8_t stm32f4_pwm_is_hardware(struct pwm *pwm) {
    /* Only hardware PWM supported, for now */
    return 1;
}

static int stm32f4_pwm_dtor(struct pwm *pwm) {
    struct pwm_ops *ops = pwm->obj.ops;
    struct stm32f4_pwm *stm32_pwm = pwm->priv;
    uint32_t all_channels = timer_all_channels(stm32_pwm->timer);

    /* Disable output */
    ops->enable(pwm, 0);

    /* Free timer channel that was taken */
    acquire(&channels_available_sem);
    channels_available |= stm32_pwm->timer_channel;

    if ((channels_available & all_channels) == all_channels) {
        /* No other channels in timer in use, disable timer */
        struct stm32f4_timer_regs *regs = timer_get_regs(stm32_pwm->timer);
        acquire(timer_semaphore(stm32_pwm->timer));
        raw_mem_clear_bits(&regs->CR1, TIM_CR1_CEN);
        release(timer_semaphore(stm32_pwm->timer));
    }

    release(&channels_available_sem);

    kfree(pwm->priv);

    if (pwm->obj.name) {
        /* Name is dynamically allocated, we can safely cast away const */
        free((char *)pwm->obj.name);
    }

    return 0;
}

static struct pwm_ops stm32f4_pwm_ops = {
    .enable = stm32f4_pwm_enable,
    .set_duty_cycle = stm32f4_pwm_set_duty_cycle,
    .get_duty_cycle = stm32f4_pwm_get_duty_cycle,
    .set_pulse_width = stm32f4_pwm_set_pulse_width,
    .get_pulse_width = stm32f4_pwm_get_pulse_width,
    .is_hardware = stm32f4_pwm_is_hardware,
    .dtor = stm32f4_pwm_dtor,
};

/*
 * Find an available timer channel for GPIO
 *
 * Search through the timer channels that may be used as a PWM output
 * for a given GPIO, and select one that is available and can be configured
 * to the correct duty cycle.
 *
 * Once taken, configure the timer as necessary.
 *
 * @param gpio  GPIO object to find a timer channel for
 * @param duty  Desired duty cycle
 * @returns timer channel taken, INVALID_TIMER_CHANNEL on error
 */
static enum timer_channels find_and_configure_timer_channel(struct gpio *gpio,
                                                            uint32_t duty) {
    uint32_t valid_channels, available;
    uint32_t selected_timer, selected_channel;
    enum timer_channels selected_timer_channel = INVALID_TIMER_CHANNEL;
    int err;

    valid_channels = gpio_to_timer_channel[gpio->num];

    /* Check for available channels */
    acquire(&channels_available_sem);

    available = valid_channels & channels_available;
    if (!available) {
        goto err_release_channels_available;
    }

    /* Loop over all channels, looking for a compatible one */
    for (int i = 0; i < PWM_NUM_CHANNELS; i++) {
        uint32_t timer_channel = (1 << i);
        if (available & timer_channel) {
            uint8_t timer = timer_channel_to_timer(timer_channel);
            acquire(timer_semaphore(timer));
            if (!timer_enabled(timer) || (timer_duty_cycle(timer) == duty)) {
                selected_timer_channel = timer_channel;
                break;
            }
            release(timer_semaphore(timer));
        }
    }

    if (selected_timer_channel == INVALID_TIMER_CHANNEL) {
        goto err_release_channels_available;
    }

    /* Configure selected timer and channel */
    selected_timer = timer_channel_to_timer(selected_timer_channel);
    selected_channel = timer_channel_to_channel(selected_timer_channel);

    if (!timer_enabled(selected_timer)) {
        err = timer_initialize(selected_timer, duty);
        if (err) {
            goto err_release_selected_timer;
        }
    }

    err = timer_configure_channel(selected_timer, selected_channel);
    if (err) {
        goto err_release_selected_timer;
    }

    release(timer_semaphore(selected_timer));

    /* Take timer channel */
    channels_available &= ~selected_timer_channel;
    release(&channels_available_sem);

    return selected_timer_channel;

err_release_selected_timer:
    release(timer_semaphore(selected_timer));
err_release_channels_available:
    release(&channels_available_sem);
    return INVALID_TIMER_CHANNEL;
}

struct obj *pwm_get(struct obj *gpio_obj, uint32_t duty) {
    struct gpio *gpio;
    struct gpio_ops *gpio_ops;
    struct obj *pwm_obj;
    struct pwm *pwm;
    struct stm32f4_pwm *stm32_pwm;
    enum timer_channels timer_channel;
    int timer, channel;
    char name_buf[32];
    char *name;
    int err, function;

    /* Verify GPIO is valid */
    if (!gpio_obj || gpio_obj->type != gpio_class.type) {
        return NULL;
    }

    gpio = to_gpio(gpio_obj);
    gpio_ops = gpio_obj->ops;

    /* Find an available timer */
    timer_channel = find_and_configure_timer_channel(gpio, duty);
    if (timer_channel == INVALID_TIMER_CHANNEL) {
        goto err;
    }

    timer = timer_channel_to_timer(timer_channel);
    channel = timer_channel_to_channel(timer_channel);

    /* Increment GPIO refcount */
    obj_get(gpio_obj);

    /* Configure alternate function */
    function = timer_gpio_af(timer);
    err = gpio_ops->set_flags(gpio, STM32F4_GPIO_ALT_FUNC, function);
    if (err) {
        goto err_put_gpio;
    }

    /* Construct PWM name */
    scnprintf(name_buf, 32, "pwm_%s", gpio_obj->name);
    name = strndup(name_buf, 32);   /* If NULL, name will simply be empty */

    /* Instantiate PWM object */
    pwm_obj = instantiate(name, &pwm_class, &stm32f4_pwm_ops, struct pwm);
    if (!pwm_obj) {
        goto err_free_name;
    }

    /* Initialize fields */
    pwm = to_pwm(pwm_obj);
    pwm->gpio = gpio_obj;
    pwm->priv = kmalloc(sizeof(struct stm32f4_pwm));
    if (!pwm->priv) {
        goto err_free_pwm_obj;
    }

    stm32_pwm = pwm->priv;
    stm32_pwm->timer_channel = timer_channel;
    stm32_pwm->timer = timer;
    stm32_pwm->channel = channel;

    class_export_member(pwm_obj);

    return pwm_obj;

err_free_pwm_obj:
    kfree(get_container(pwm_obj));
err_free_name:
    if (name) {
        free(name);
    }

    /* Free timer channel that was taken */
    acquire(&channels_available_sem);
    channels_available |= timer_channel;
    release(&channels_available_sem);
err_put_gpio:
    obj_put(gpio_obj);
err:
    return NULL;
}
