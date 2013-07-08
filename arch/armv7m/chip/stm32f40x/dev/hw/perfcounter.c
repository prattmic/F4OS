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
