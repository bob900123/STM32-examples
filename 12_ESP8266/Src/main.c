#include "Com_Utils.h"
#include "Driver_ADC.h"
#include "Driver_ESP8266.h"
#include "Driver_HTTP.h"
#include "Driver_USART.h"
#include <stdio.h>
#include <stm32f10x.h>
#include <string.h>

#define VREF 3.28f

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

int main(void)
{
    uint16_t adc_val = 0;
    float voltage = 0;
    // char str[30];

    STM32_Init();
    Driver_USART_Init();
    Driver_ADC_Init();
    Driver_HTTP_Init();

    Driver_USART_SendString("Init Success\n", 13);

    while (1)
    {
        adc_val = Driver_ADC_GetVal();
        voltage = ((float)adc_val / 4095.0f) * VREF;
        // int l = sprintf(str, "ADC: %.3f\n", voltage);
        // Driver_USART_SendString((uint8_t *)str, (uint16_t)l);
        Driver_HTTP_Post(voltage);
        Com_Utils_DelayMs(500);
    }
}