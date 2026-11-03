#include "Driver_Button.h"
#include "Com_Utils.h"
#include "Driver_USART.h"

bool button_flag = false;

void Driver_Button_Init()
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    GPIOA->CRL &= ~GPIO_CRL_MODE2;
    GPIOA->CRL &= ~GPIO_CRL_CNF2_0;
    GPIOA->CRL |= GPIO_CRL_CNF2_1;
    GPIOA->ODR &= ~GPIO_ODR_ODR2; // 下拉電阻

    AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI2_PA;

    EXTI->IMR |= EXTI_IMR_MR2;
    EXTI->RTSR |= EXTI_RTSR_TR2;

    // NVIC
    NVIC_SetPriorityGrouping(3);
    NVIC_SetPriority(EXTI2_IRQn, 3);
    NVIC_EnableIRQ(EXTI2_IRQn);
}

void EXTI2_IRQHandler()
{
    EXTI->PR |= EXTI_PR_PR2;
    Driver_USART_SendString("button is pressed...\r\n", 22);
    button_flag = true;
}