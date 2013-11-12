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

#include <arch/chip/registers.h>
#include <dev/hw/perfcounter.h>

/* 64-bit timer, with TIM5 slave to TIM2 */

/*
 * APB1 clock is system clock / 4.
 * Timers on APB1 are clocked at 2 * APB1 clock.
 */
#define TIMER_PRESCALER (2)

static void init_tim2(void) {
    /* Enable timer clock */
    *RCC_APB1ENR |= RCC_APB1ENR_TIM2EN;

    /* No prescale, max count */
    *TIM2_PSC = 0;
    *TIM2_ARR = 0xffffffff;

    /* TIM2 set update event as output in master control reg */
    *TIM2_CR2 |= (1 << 5);

    /* Enable timers */
    *TIM2_CR1 |= TIMx_CR1_CEN;
}

static void init_tim5(void) {
    /* Enable timer clock */
    *RCC_APB1ENR |= RCC_APB1ENR_TIM5EN;

    /* No prescale, max count */
    *TIM5_PSC = 0;
    *TIM5_ARR = 0xffffffff;

    /* External clock mode in TIM5 */
    *TIM5_SMCR |= 0x7;

    /* Enable timers */
    *TIM5_CR1 |= TIMx_CR1_CEN;
}

void init_perfcounter(void) {
    init_tim5();
    init_tim2();
}

uint64_t perfcounter_getcount(void) {
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
        upper = *TIM5_CNT;
        lower = *TIM2_CNT;
    } while(upper != *TIM5_CNT);

    count = ((uint64_t)upper << 32) | lower;

    /* Return in system clock ticks */
    return TIMER_PRESCALER * count;
}
