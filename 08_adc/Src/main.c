#include "Driver_ADC.h"
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

#define VREF 3.36f

int main(void)
{
    uint16_t adc_val = 0;
    float voltage = 0;
    char str[30];

    STM32_Init();
    Driver_USART_Init((uint32_t)buffer);
    Driver_ADC_Init();

    while (1)
    {
        adc_val = Driver_ADC_GetVal();
        voltage = ((float)adc_val / 4095.0f) * VREF;
        sprintf(str, "ADC: %.3f\n", voltage);
        // sprintf(str, "ADC: %d\n", adc_val);
        Driver_USART_SendString((uint8_t *)str, (uint16_t)strlen(str));
        delay(2048);
    }
}