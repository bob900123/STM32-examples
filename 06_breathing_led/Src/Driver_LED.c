#include "Driver_LED.h"
#include "Driver_Timer.h"

void Driver_LED_Init()
{
    Driver_Timer_Init();

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRL |= GPIO_CRL_MODE6;
    GPIOA->CRL &= ~GPIO_CRL_CNF6_0;
    GPIOA->CRL |= GPIO_CRL_CNF6_1;

    GPIOA->ODR &= ~LED;
}

void Driver_LED_Brightness(uint8_t brightness)
{
    Driver_Timer_CC1Value(brightness);
}
