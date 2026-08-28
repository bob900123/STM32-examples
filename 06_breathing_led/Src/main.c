#include "Com_Delay.h"
#include "Driver_LED.h"
#include <stm32f10x.h>

void STM32_Init()
{
    RCC->CR |= RCC_CR_HSEON;
    RCC->CFGR |= RCC_CFGR_PLLSRC;
    RCC->CFGR |= RCC_CFGR_PLLMULL9;
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    RCC->CR |= RCC_CR_PLLON;

    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
}

int main(void)
{
    STM32_Init();
    Driver_LED_Init();

    while (1)
    {
        // for (int8_t i = 0; i < 100; i++)
        // {
        //     Driver_LED_Brightness(i);
        //     Com_DelayMs(50);
        // }
        // for (int8_t i = 99; i > -1; i--)
        // {
        //     Driver_LED_Brightness(i);
        //     Com_DelayMs(50);
        // }
    }
}