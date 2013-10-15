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

static void init_tim1(void) {
    /* Enable timer clock */
    *RCC_APB2ENR |= RCC_APB2ENR_TIM1EN;

    /* No prescale, max count */
    *TIM1_PSC = 0;
    *TIM1_ARR = 0xffff;

    /* TIM1 set update event as output in master control reg */
    *TIM1_CR2 |= (1 << 5);

    /* Enable timers */
    *TIM1_CR1 |= TIMx_CR1_CEN;
}

static void init_tim2(void) {
    /* Enable timer clock */
    *RCC_APB1ENR |= RCC_APB1ENR_TIM2EN;

    /* No prescale, max count */
    *TIM2_PSC = 0;
    *TIM2_ARR = 0xffffffff;

    /* External clock mode in TIM2 */
    *TIM2_SMCR |= 0x7;

    /* Enable timers */
    *TIM2_CR1 |= TIMx_CR1_CEN;
}

void init_perfcounter(void) {
    init_tim2();
    init_tim1();
}

inline uint64_t perfcounter_getcount(void) {
    return (*TIM2_CNT << 16)|(*TIM1_CNT);
}
