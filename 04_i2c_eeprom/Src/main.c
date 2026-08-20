#include "Com_Utils.h"
#include "Driver_EEPROM.h"
#include "Driver_I2C.h"
#include "Driver_USART.h"
#include <stdio.h>
#include <stm32f10x.h>
#include <string.h>

void STM32_Init()
{
    RCC->CR |= RCC_CR_HSEON;
    RCC->CFGR |= RCC_CFGR_PLLSRC;
    RCC->CFGR |= RCC_CFGR_PLLMULL9;
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    RCC->CR |= RCC_CR_PLLON;

    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
}

int fputc(int c, FILE *file)
{
    Driver_USART_SendChar(c);
    return c;
}

int main(void)
{
    STM32_Init();
    Driver_USART_Init();
    Driver_EEPROM_Init();

    uint8_t receive_bytes[100];
    uint8_t ack = 0;
    uint8_t address = 0xB0;

    while (1)
    {
        if (is_finish)
        {
            if (strncmp((char *)buffer, "Read:", 5) == 0)
            {
                uint8_t count = Com_Utils_GetLength((char *)buffer, length);
                ack = Driver_EEPROM_ReadBytes(address, receive_bytes, count);
                if (ack == FAIL)
                {
                    Driver_USART_SendString("Read Fail\n", 10);
                }
                else
                {
                    Driver_USART_SendString(receive_bytes, count);
                    Driver_USART_SendString("\n", 1);
                }
            }
            else if (strncmp((char *)buffer, "Write:", 6) == 0)
            {
                ack = Driver_EEPROM_WriteBytes(address, buffer + 6, length - 6);
                Driver_USART_SendString((uint8_t *)(ack == FAIL ? "Write Fail\n" : "Write Done\n"), 11);
            }
            else
            {
                Driver_USART_SendString("Unknow\n", 7);
            }

            is_finish = false;
            length = 0;
        }
    }
}
