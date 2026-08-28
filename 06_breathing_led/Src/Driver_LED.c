#include "Driver_LED.h"
#include "Driver_Timer.h"

void Driver_LED_Init()
{
    Driver_Timer_Init();

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRH |= GPIO_CRH_MODE8;
    GPIOA->CRH &= ~GPIO_CRH_CNF8_0;
    GPIOA->CRH |= GPIO_CRH_CNF8_1;
}