#include "Driver_LED.h"

void Driver_LED_Init()
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // PA5 - LED1
    GPIOA->CRL |= GPIO_CRL_MODE5;
    GPIOA->CRL &= ~GPIO_CRL_CNF5;

    // PA6 - LED2
    GPIOA->CRL |= GPIO_CRL_MODE6;
    GPIOA->CRL &= ~GPIO_CRL_CNF6;

    // PA7 - LED3
    GPIOA->CRL |= GPIO_CRL_MODE7;
    GPIOA->CRL &= ~GPIO_CRL_CNF7;

    Driver_LED_Off(LED1);
    Driver_LED_Off(LED2);
    Driver_LED_Off(LED3);
}

void Driver_LED_On(uint16_t LED)
{
    GPIOA->ODR |= LED;
}

void Driver_LED_Off(uint16_t LED)
{
    GPIOA->ODR &= ~LED;
}

void Driver_LED_Toggle(uint16_t LED)
{
    if ((GPIOA->ODR & LED) == 0)
    {
        Driver_LED_On(LED);
    }
    else
    {
        Driver_LED_Off(LED);
    }
}

uint8_t Driver_LED_Status(uint16_t LED)
{
    if ((GPIOA->ODR & LED) == 0)
    {
        return LED_OFF;
    }
    return LED_ON;
}