#include "Com_Utils.h"
#include "Driver_LED.h"
#include "Driver_TCP.h"
#include "Driver_USART.h"
#include "Driver_W5500.h"
#include <stdio.h>
#include <stm32f10x.h>
#include <string.h>

char usart_str[100];
uint16_t usart_len = 0;
uint8_t rxBuff[1024] = {0};
uint16_t rxLen = 0;

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
    Driver_USART_Init((uint32_t)buffer);
    Driver_LED_Init();
    Driver_W5500_Init();

    while (1)
    {
        Driver_TCP_ClientStart();
        Driver_TCP_RecvData(rxBuff, &rxLen);

        if (rxLen > 0)
        {
            usart_len = sprintf(usart_str, "Get Message: %s\n", (char *)rxBuff);
            Driver_USART_SendString((uint8_t *)usart_str, usart_len);
            if (strcmp((char *)rxBuff, "on") == 0)
            {
                Driver_LED_On();
                Driver_TCP_SendData("LED ON", 6);
            }
            else if (strcmp((char *)rxBuff, "off") == 0)
            {
                Driver_LED_Off();
                Driver_TCP_SendData("LED OFF", 7);
            }
            rxLen = 0;
        }
    }
}