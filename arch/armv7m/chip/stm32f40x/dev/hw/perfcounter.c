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

#include <limits.h>
#include <arch/chip/registers.h>
#include <arch/chip/timer.h>
#include <dev/hw/perfcounter.h>
#include <dev/raw_mem.h>

/* 64-bit timer, with TIM5 slave to TIM2 */

/*
 * APB1 clock is system clock / 4.
 * Timers on APB1 are clocked at 2 * APB1 clock.
 */
#define TIMER_PRESCALER (2)

/* Acts as master timer, sends update event */
static void init_tim2(void) {
    struct stm32f4_timer_regs *regs = timer_get_regs(2);

    /* Enable timer clock */
    *RCC_APB1ENR |= RCC_APB1ENR_TIM2EN;

    /* No clock prescale, max count */
    raw_mem_write(&regs->PSC, 0);

    /* Maximum count value */
    raw_mem_write(&regs->ARR, UINT32_MAX);

    /* Set update event as output in master control reg */
    raw_mem_set_mask(&regs->CR2, TIM_CR2_MMS_MASK, TIM_CR2_MMS_UP);

    /* Enable timer */
    raw_mem_set_bits(&regs->CR1, TIM_CR1_CEN);
}

/* Acts as slave timer, incrementing on update event */
static void init_tim5(void) {
    struct stm32f4_timer_regs *regs = timer_get_regs(5);

    /* Enable timer clock */
    *RCC_APB1ENR |= RCC_APB1ENR_TIM5EN;

    /* No clock prescale, max count */
    raw_mem_write(&regs->PSC, 0);

    /* Maximum count value */
    raw_mem_write(&regs->ARR, UINT32_MAX);

    /* External clock slave mode */
    raw_mem_set_bits(&regs->SMCR, TIM_SMCR_SMS_EXT);

    /* Enable timer */
    raw_mem_set_bits(&regs->CR1, TIM_CR1_CEN);
}

void init_perfcounter(void) {
    init_tim5();
    init_tim2();
}

uint64_t perfcounter_getcount(void) {
    struct stm32f4_timer_regs *tim2 = timer_get_regs(2);
    struct stm32f4_timer_regs *tim5 = timer_get_regs(5);
    uint32_t upper, lower;
    uint64_t count;

    /*
     * Ensure atomic read of complete upper + lower value.
     *
     * If lower overflows between reading upper and lower, the value of upper
     * is no longer valid, so it will not match the register, and another
     * attempt is made.
     *
     * The lower timer must overflow suffciently slowly!
     */
    do {
        upper = raw_mem_read(&tim5->CNT);
        lower = raw_mem_read(&tim2->CNT);
    } while(upper != raw_mem_read(&tim5->CNT));

    count = ((uint64_t)upper << 32) | lower;

    /* Return in system clock ticks */
    return TIMER_PRESCALER * count;
}
