#include "Com_Utils.h"
#include "Driver_ADC.h"
#include "Driver_LED.h"
#include "Driver_Lora.h"
#include "Driver_USART.h"
#include <stdbool.h>
#include <stdio.h>
#include <stm32f10x.h>
#include <string.h>

char usart_buffer[100];
uint16_t usart_len = 0;
char lora_buffer[256];
uint16_t lora_len = 0;

bool ParseBrightness(const char *data, uint16_t len, uint8_t *brightness)
{
    uint16_t value = 0;

    /* The transmitter appends a newline; accept CR/LF at the packet end. */
    while (len > 0 && (data[len - 1] == '\r' || data[len - 1] == '\n'))
    {
        len--;
    }
    if (len == 0)
    {
        return false;
    }

    for (uint16_t i = 0; i < len; i++)
    {
        if (data[i] < '0' || data[i] > '9')
        {
            return false;
        }
        value = value * 10 + (uint16_t)(data[i] - '0');
        if (value > 100)
        {
            return false;
        }
    }

    *brightness = (uint8_t)value;
    return true;
}

void STM32_Init(void)
{
    RCC->CR |= RCC_CR_HSEON;
    while ((RCC->CR & RCC_CR_HSERDY) == 0)
        ;

    // PLL = 8 MHz * 9 = 72 MHz
    RCC->CFGR |= RCC_CFGR_PLLSRC;
    RCC->CFGR |= RCC_CFGR_PLLMULL9;

    // AHB = SYSCLK / 1 = 72 MHz
    RCC->CFGR &= ~RCC_CFGR_HPRE;

    // APB1 = HCLK / 2 = 36 MHz
    RCC->CFGR &= ~RCC_CFGR_PPRE1;
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

    // APB2 = HCLK / 1 = 72 MHz
    RCC->CFGR &= ~RCC_CFGR_PPRE2;

    RCC->CR |= RCC_CR_PLLON;
    while ((RCC->CR & RCC_CR_PLLRDY) == 0)
        ;

    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
        ;
}

int fputc(int c, FILE *file)
{
    uint8_t ch = (uint8_t)c;
    Driver_USART_SendString(&ch, 1);
    return c;
}

// Receive
// int main(void)
// {
//     STM32_Init();
//     Driver_LED_Init();
//     Driver_LED_SetBrightness(50);
//     Driver_USART_Init((uint32_t)buffer);
//     Driver_USART_SendString("Start Init\n", 11);
//     uint8_t result = Driver_Lora_Init();
//     usart_len = sprintf(usart_buffer, "Result: %d\n", result);
//     Driver_USART_SendString((uint8_t *)usart_buffer, usart_len);

//     result = Driver_Lora_EnterRxMode();
//     usart_len = sprintf(usart_buffer, "RxMode: %d\n", result);
//     Driver_USART_SendString((uint8_t *)usart_buffer, usart_len);

//     while (1)
//     {
//         Driver_Lora_RecvData((uint8_t *)lora_buffer, &lora_len);
//         if (lora_len > 0)
//         {
//             uint8_t brightness;
//             if (ParseBrightness(lora_buffer, lora_len, &brightness))
//             {
//                 Driver_LED_SetBrightness(brightness);
//                 printf("Brightness: %u%%\r\n", (unsigned int)brightness);
//             }
//             else
//             {
//                 printf("Invalid brightness: expected 0-100\r\n");
//             }
//             lora_len = 0;
//         }
//         Com_Utils_DelayMs(10);
//     }
// }

// Transmit
int main(void)
{
    STM32_Init();
    Driver_ADC_Init();
    Driver_USART_Init((uint32_t)buffer);
    Driver_USART_SendString("Start Init\n", 11);
    uint8_t result = Driver_Lora_Init();
    usart_len = sprintf(usart_buffer, "Result: %d\n", result);
    Driver_USART_SendString((uint8_t *)usart_buffer, usart_len);

    if (result != 0)
    {
        while (1)
            ;
    }

    while (1)
    {
        uint16_t raw = Driver_ADC_ReadAverage();
        uint8_t brightness = Driver_ADC_ToPercentage(raw);

        printf("brightness: %lu%%\r\n", (unsigned long)brightness);
        uint8_t msg[10] = {0};
        int l = sprintf((char *)msg, "%lu\n", (unsigned long)brightness);
        result = Driver_Lora_SendData(msg, l);
        Com_Utils_DelayMs(500);
    }
}
