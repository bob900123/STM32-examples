#include "Driver_LED.h"
#include "Driver_USART.h"
#include <stdbool.h>
#include <stm32f10x.h>
#include <string.h>

void STM32_Init()
{
    RCC->CR |= RCC_CR_HSEON;
    RCC->CFGR |= RCC_CFGR_PLLSRC;
    RCC->CFGR |= RCC_CFGR_PLLMULL9;
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    RCC->CR |= RCC_CR_PLLON;
}

void delay(unsigned int ms)
{
    unsigned int i;

    while (ms--)
    {
        for (i = 0; i < 8000; i++)
        {
        }
    }
}

int main(void)
{
    STM32_Init();
    Driver_USART_Init();
    Driver_LED_Init();

    while (1)
    {
        if (is_finish)
        {
            if (strcmp((char *)buffer, "on") == 0)
            {
                Driver_LED_On();
                Driver_USART_SendString("LED ON\n", 7);
            }
            else if (strcmp((char *)buffer, "off") == 0)
            {
                Driver_LED_Off();
                Driver_USART_SendString("LED OFF\n", 8);
            }
            else if (strcmp((char *)buffer, "toggle") == 0)
            {
                Driver_LED_Toggle();
                Driver_USART_SendString("LED Toggle\n", 11);
            }
            else
            {
                Driver_USART_SendString("UNKNOWN\n", 8);
            }

            is_finish = false;
            length = 0;
        }
    }
}