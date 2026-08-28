#include "Driver_Timer.h"
#include "stm32f10x.h"

uint8_t RCR_VALUE = 2;
int8_t current_brightness = 0;
int8_t direction = 1;

void Driver_Timer_Init()
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    TIM1->CR1 |= TIM_CR1_ARPE;
    TIM1->PSC = 7200 - 1;

    TIM1->DIER |= TIM_DIER_UIE;
    TIM1->BDTR |= TIM_BDTR_MOE;

    TIM1->CCMR1 &= ~TIM_CCMR1_OC1M_0;
    TIM1->CCMR1 |= TIM_CCMR1_OC1M_1;
    TIM1->CCMR1 |= TIM_CCMR1_OC1M_2;

    TIM1->CCER |= TIM_CCER_CC1E;
    TIM1->ARR = 100 - 1;

    TIM1->RCR = RCR_VALUE;

    NVIC_SetPriorityGrouping(3);
    NVIC_SetPriority(TIM1_UP_IRQn, 2);
    NVIC_EnableIRQ(TIM1_UP_IRQn);

    TIM1->CR1 |= TIM_CR1_CEN;
}

void TIM1_UP_IRQHandler(void)
{
    TIM1->SR &= ~TIM_SR_UIF;
    current_brightness += direction;

    if (current_brightness >= 100)
    {
        current_brightness = 100;
        direction = -1;
    }
    else if (current_brightness <= 0)
    {
        current_brightness = 0;
        direction = 1;
    }

    TIM1->CCR1 = current_brightness;
}
