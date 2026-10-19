#include "Driver_LED.h"

void Driver_LED_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_TIM1EN;
    RCC->APB2RSTR |= RCC_APB2RSTR_TIM1RST;
    RCC->APB2RSTR &= ~RCC_APB2RSTR_TIM1RST;

    AFIO->MAPR &= ~AFIO_MAPR_TIM1_REMAP;
    GPIOB->BRR = GPIO_BRR_BR13;
    GPIOB->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
    GPIOB->CRH |= GPIO_CRH_MODE13_1 | GPIO_CRH_CNF13_1;

    TIM1->PSC = 72 - 1;
    TIM1->ARR = 1000 - 1;
    TIM1->CCR1 = 0;
    TIM1->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE;
    TIM1->CR1 = TIM_CR1_ARPE;
    TIM1->EGR = TIM_EGR_UG;
    TIM1->SR = 0;

    TIM1->CCER = TIM_CCER_CC1NE;
    TIM1->BDTR = TIM_BDTR_MOE;
    TIM1->CR1 |= TIM_CR1_CEN;
}

void Driver_LED_SetBrightness(uint8_t brightness)
{
    if (brightness > 100)
    {
        brightness = 100;
    }

    TIM1->CCR1 = (uint32_t)brightness * 10;
}

void Driver_LED_On(void)
{
    Driver_LED_SetBrightness(100);
}

void Driver_LED_Off(void)
{
    Driver_LED_SetBrightness(0);
}
