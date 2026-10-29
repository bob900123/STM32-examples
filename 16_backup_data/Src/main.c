#include "Com_Utils.h"
#include "Driver_BKP.h"
#include "Driver_USART.h"
#include <stdbool.h>
#include <stdio.h>
#include <stm32f10x.h>
#include <string.h>

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

    uint8_t data[20] = {0};

    Driver_BKP_Read(data, 1);
    if (data[0] == 0x00)
    {
        Driver_BKP_Write("Last Xuan God\n", 14);
        Driver_USART_SendString("Backup succeeded\n", 17);
    }
    else
    {
        Driver_USART_SendString("Already backed up\n", 18);
    }

    while (1)
    {
        Driver_BKP_Read(data, 14);
        Driver_USART_SendString(data, 14);
        Com_Utils_DelayS(1);
    }
}