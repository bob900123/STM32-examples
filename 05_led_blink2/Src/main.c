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
}

int main(void)
{
    STM32_Init();
    Driver_LED_Init();

    while (1)
    {
        Driver_LED_Toggle();
        Com_DelayMs(100);
    }
}