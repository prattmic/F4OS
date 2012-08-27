#include <dev/registers.h>
#include <dev/hw/tim.h>

void init_tim2(void) {
    /* Enable TIM2 clock */
    *RCC_APB1ENR |= RCC_APB1ENR_TIM2EN;

    /* Enable interrupt */
    *NVIC_ISER0 |= (1 << 28);
    *TIM2_DIER |= TIM2_DIER_UIE;

    *TIM2_PSC = 84-1;
    *TIM2_ARR = 100-1;  /* Period in microseconds */

    *TIM2_CR1 |= TIM2_CR1_ARPE;

    /* Enable timer */
    *TIM2_CR1 |= TIM2_CR1_CEN;
    *TIM2_EGR = 1;
}
