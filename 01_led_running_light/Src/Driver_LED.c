#include "Driver_LED.h"

void Driver_LED_Init(void)
{
    // GPIO 設定
    RCC->APB2ENR = RCC_APB2ENR_IOPAEN;
    GPIOA->CRL |= (GPIO_CRL_MODE0 | GPIO_CRL_MODE1 | GPIO_CRL_MODE2 | GPIO_CRL_MODE3 | GPIO_CRL_MODE4);
    GPIOA->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_CNF1 | GPIO_CRL_CNF2 | GPIO_CRL_CNF3 | GPIO_CRL_CNF4);

    Driver_LED_Off(LED_1);
    Driver_LED_Off(LED_2);
    Driver_LED_Off(LED_3);
    Driver_LED_Off(LED_4);
    Driver_LED_Off(LED_5);
}

void Driver_LED_On(uint32_t led)
{
    GPIOA->ODR |= led;
}

void Driver_LED_Off(uint32_t led)
{
    GPIOA->ODR &= ~led;
}

void Driver_LED_Toggle(uint32_t led)
{
    if ((GPIOA->ODR & led) == 0)
    {
        Driver_LED_On(led);
    }
    else
    {
        Driver_LED_Off(led);
    }
}

void Driver_LED_OnAll(uint32_t leds[], uint8_t size)
{
    for (int i = 0; i < size; i++)
    {
        Driver_LED_On(leds[i]);
    }
}

void Driver_LED_OffAll(uint32_t leds[], uint8_t size)
{
    for (int i = 0; i < size; i++)
    {
        Driver_LED_Off(leds[i]);
    }
}