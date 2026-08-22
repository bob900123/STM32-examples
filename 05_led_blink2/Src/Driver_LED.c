#include "Driver_LED.h"
#include "stm32f10x.h"

void Driver_LED_Init()
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRL |= GPIO_CRL_MODE2;
    GPIOA->CRL &= ~GPIO_CRL_CNF2;

    GPIOA->ODR &= ~GPIO_ODR_ODR2;
}

void Driver_LED_On()
{
    GPIOA->ODR |= GPIO_ODR_ODR2;
}

void Driver_LED_Off()
{
    GPIOA->ODR &= ~GPIO_ODR_ODR2;
}

void Driver_LED_Toggle()
{
    if ((GPIOA->IDR & GPIO_IDR_IDR2) == 0)
    {
        Driver_LED_On();
    }
    else
    {
        Driver_LED_Off();
    }
}