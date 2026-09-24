#include "Driver_LED.h"

void Driver_LED_Init()
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRL |= GPIO_CRL_MODE1;
    GPIOA->CRL &= ~GPIO_CRL_CNF1;

    Driver_LED_Off(LED);
}

void Driver_LED_On()
{
    GPIOA->ODR |= LED;
}

void Driver_LED_Off()
{
    GPIOA->ODR &= ~LED;
}

void Driver_LED_Toggle()
{
    if ((GPIOA->ODR & LED) == 0)
    {
        Driver_LED_On();
    }
    else
    {
        Driver_LED_Off();
    }
}