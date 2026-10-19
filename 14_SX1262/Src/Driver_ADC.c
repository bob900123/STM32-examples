#include "Driver_ADC.h"
#include "Com_Utils.h"
#include "stm32f10x.h"

void Driver_ADC_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_ADC1EN;
    /* PCLK2 = 72 MHz; ADC clock = 12 MHz (maximum allowed: 14 MHz). */
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_ADCPRE) | RCC_CFGR_ADCPRE_DIV6;
    GPIOB->CRL &= ~(GPIO_CRL_MODE1 | GPIO_CRL_CNF1); /* PB1: analog input */

    RCC->APB2RSTR |= RCC_APB2RSTR_ADC1RST;
    RCC->APB2RSTR &= ~RCC_APB2RSTR_ADC1RST;
    ADC1->SMPR2 = ADC_SMPR2_SMP9;
    ADC1->SQR1 = 0;
    ADC1->SQR3 = 9;
    ADC1->CR2 = ADC_CR2_EXTSEL | ADC_CR2_EXTTRIG | ADC_CR2_ADON;
    Com_Utils_DelayMs(1);

    ADC1->CR2 |= ADC_CR2_RSTCAL;
    while (ADC1->CR2 & ADC_CR2_RSTCAL)
        ;
    ADC1->CR2 |= ADC_CR2_CAL;
    while (ADC1->CR2 & ADC_CR2_CAL)
        ;
}

uint16_t Driver_ADC_ReadAverage(void)
{
    uint32_t sum = 0;

    for (uint8_t i = 0; i < 16; i++)
    {
        ADC1->CR2 |= ADC_CR2_SWSTART;
        while ((ADC1->SR & ADC_SR_EOC) == 0)
            ;
        sum += ADC1->DR;
    }
    return (uint16_t)((sum + 8U) / 16U);
}

uint32_t Driver_ADC_ToMillivolts(uint16_t raw)
{
    return ((uint32_t)raw * DRIVER_ADC_REFERENCE_MV + DRIVER_ADC_FULL_SCALE / 2U) / DRIVER_ADC_FULL_SCALE;
}

uint8_t Driver_ADC_ToPercentage(uint16_t raw)
{
    uint32_t voltage_mv = Driver_ADC_ToMillivolts(raw);
    uint32_t percentage = (voltage_mv * 100U + DRIVER_ADC_INPUT_MAX_MV / 2U) / DRIVER_ADC_INPUT_MAX_MV;
    if (percentage > 100U)
    {
        percentage = 100U;
    }
    return (uint8_t)percentage;
}
