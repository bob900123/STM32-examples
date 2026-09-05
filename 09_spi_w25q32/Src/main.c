#include "Com_Utils.h"
#include "Driver_Flash.h"
#include "Driver_USART.h"
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

void delay(unsigned int count)
{
    unsigned int i;

    while (count--)
    {
        for (i = 0; i < 8000; i++)
        {
        }
    }
}

int main(void)
{
    uint8_t mid, did;
    char str[100];

    uint8_t block = 7;
    uint8_t sector = 7;
    uint8_t page = 7;

    STM32_Init();
    Driver_USART_Init((uint32_t)buffer);
    Driver_Flash_Init();
    Driver_Flash_ReadId(&mid, &did);

    sprintf(str, "Manufacturer ID: %u\nDevice ID: %u\n", mid, did);
    Driver_USART_SendString((uint8_t *)str, strlen(str));

    while (1)
    {
        if (is_receive_finished)
        {
            if (strncmp((char *)buffer, "Read:", 5) == 0)
            {
                uint8_t count = Com_Utils_GetLength((char *)buffer, length);
                Driver_Flash_Read(block, sector, page, (uint8_t *)str, count);
                Driver_USART_SendString((uint8_t *)str, count);
                Driver_USART_SendString("\n", 1);
            }
            else if (strncmp((char *)buffer, "Write:", 6) == 0)
            {
                Driver_Flash_WritePage(block, sector, page, buffer + 6, length - 6);
                Driver_USART_SendString("Write Done\n", 11);
            }
            else
            {
                Driver_USART_SendString("Unknow\n", 7);
            }

            is_receive_finished = false;
            length = 0;
        }
    }
}