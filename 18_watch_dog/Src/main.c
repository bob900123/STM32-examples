#include "Com_Utils.h"
#include "Driver_Button.h"
#include "Driver_IWDG.h"
#include "Driver_USART.h"
#include <stdio.h>
#include <stm32f10x.h>
#include <string.h>
#include <time.h>

void STM32_Init(void)
{
    RCC->CR |= RCC_CR_HSEON;
    while ((RCC->CR & RCC_CR_HSERDY) == 0)
        ;

    RCC->CFGR |= RCC_CFGR_PLLSRC;
    RCC->CFGR |= RCC_CFGR_PLLMULL9;

    RCC->CR |= RCC_CR_PLLON;
    while ((RCC->CR & RCC_CR_PLLRDY) == 0)
        ;

    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
        ;
}

int main(void)
{
    STM32_Init();
    Driver_USART_Init();
    Driver_Button_Init();
    Driver_IWDG_Init();

    Driver_USART_SendString("Good Morning!!!\r\n", 17);

    while (1)
    {
        Driver_USART_SendString("working...\r\n", 12);
        Com_Utils_DelayS(3);

        if (button_flag)
        {
            button_flag = false;
            Driver_USART_SendString("Dog Died (｡>﹏<｡) (╥﹏╥)\r\n", strlen("Dog Died (｡>﹏<｡) (╥﹏╥)\r\n"));
            Com_Utils_DelayS(3);
        }

        Driver_USART_SendString("feed the dog...\r\n", 17);
        Driver_IWDG_Refresh();
    }
}
