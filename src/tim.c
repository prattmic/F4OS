#include "types.h"
#include "registers.h"
#include "usart.h"
#include "tim.h"

void init_timer(void) {
    /* Enable TIM2 clock */
    *RCC_APB1ENR |= RCC_APB1ENR_TIM2EN;

    *TIM2_CR1 |= TIM2_CR1_ARPE;
    *TIM2_CR2 |= TIM2_CR2_MMS_EN;

    /* Enable interrupt */
    *NVIC_ISER0 |= (1 << 28);
    *TIM2_DIER |= TIM2_DIER_UIE;

    *TIM2_ARR = 100000;
    *TIM2_PSC = 100;

    /* Enable timer */
    *TIM2_CR1 |= TIM2_CR1_CEN;
}

void tim2_handler(void) {
}
