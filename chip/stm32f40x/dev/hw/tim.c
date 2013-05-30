#include <dev/registers.h>
#include <dev/cortex_m.h>
#include <dev/hw/tim.h>

void init_tim2(void) {
    /* Enable timer clocks */
    *RCC_APB2ENR |= RCC_APB2ENR_TIM1EN;
    *RCC_APB1ENR |= RCC_APB1ENR_TIM2EN;

    /* No prescale, max count */
    *TIM2_PSC = 0;
    *TIM2_ARR = 0xffffffff;
    *TIM1_PSC = 0;
    *TIM1_ARR = 0xffff;

    /* TIM1 set update event as output in master control reg */
    *TIM1_CR2 |= (1 << 5);
    /* External clock mode in TIM2 */
    *TIM2_SMCR |= 0x7;


    /* Enable timers */
    *TIM2_CR1 |= TIMx_CR1_CEN;
    *TIM1_CR1 |= TIMx_CR1_CEN;

    *TIM2_EGR = 1;
}
