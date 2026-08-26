#include "Driver_Timer.h"

void Driver_Timer_Init()
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    TIM3->CR1 |= TIM_CR1_ARPE;

    TIM3->CCMR1 &= ~TIM_CCMR1_OC1M_0;
    TIM3->CCMR1 |= TIM_CCMR1_OC1M_1;
    TIM3->CCMR1 |= TIM_CCMR1_OC1M_2;

    TIM3->CCER |= TIM_CCER_CC1E;
    TIM3->ARR = 100 - 1;

    TIM3->CR1 |= TIM_CR1_CEN;
}

void Driver_Timer_CC1Value(uint16_t value)
{
    TIM3->CCR1 = value;
}