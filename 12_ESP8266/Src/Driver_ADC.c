#include "Driver_ADC.h"

void Driver_ADC_Init()
{
    // 啟用時鐘
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->CFGR |= RCC_CFGR_ADCPRE_DIV6;

    // GPIOA0
    GPIOA->CRL &= ~GPIO_CRL_MODE0; // 輸入
    GPIOA->CRL &= ~GPIO_CRL_CNF0;  // 類比輸入

    ADC1->CR1 |= ADC_CR1_SCAN;

    ADC1->CR2 |= ADC_CR2_CONT;

    ADC1->CR2 |= ADC_CR2_EXTSEL;
    ADC1->CR2 |= ADC_CR2_EXTTRIG;

    ADC1->SQR1 &= ~ADC_SQR1_L;
    ADC1->SQR3 &= ~ADC_SQR3_SQ1;

    ADC1->SMPR2 &= ~ADC_SMPR2_SMP0;
    ADC1->SMPR2 |= ADC_SMPR2_SMP0;

    ADC1->CR2 |= ADC_CR2_ADON;

    ADC1->CR2 |= ADC_CR2_RSTCAL;
    while (ADC1->CR2 & ADC_CR2_RSTCAL)
        ;
    ADC1->CR2 |= ADC_CR2_CAL;
    while (ADC1->CR2 & ADC_CR2_CAL)
        ;
    ADC1->CR2 |= ADC_CR2_SWSTART;
}

uint16_t Driver_ADC_GetVal()
{
    // ADC1->CR2 |= ADC_CR2_SWSTART;
    return (uint16_t)(ADC1->DR & ADC_DR_DATA);
}