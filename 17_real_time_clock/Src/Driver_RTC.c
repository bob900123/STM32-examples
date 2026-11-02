#include "Driver_RTC.h"

#define RTC_INIT_FLAG 0xA5A5

void Driver_RTC_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    RCC->APB1ENR |= RCC_APB1ENR_BKPEN;
    PWR->CR |= PWR_CR_DBP;

    // 啟動 LSE
    RCC->BDCR |= RCC_BDCR_LSEON;
    while (!(RCC->BDCR & RCC_BDCR_LSERDY))
        ;

    RCC->BDCR &= ~RCC_BDCR_RTCSEL;
    RCC->BDCR |= RCC_BDCR_RTCSEL_LSE;
    RCC->BDCR |= RCC_BDCR_RTCEN;

    RTC->CRL &= ~RTC_CRL_RSF;
    while (!(RTC->CRL & RTC_CRL_RSF))
        ;
    while (!(RTC->CRL & RTC_CRL_RTOFF))
        ;

    RTC->CRL |= RTC_CRL_CNF;
    RTC->PRLH = 0;
    RTC->PRLL = 32767;
    RTC->CRL &= ~RTC_CRL_CNF;
    while (!(RTC->CRL & RTC_CRL_RTOFF))
        ;

    PWR->CR &= ~PWR_CR_DBP;
}

void Driver_RTC_SetTime(uint32_t time)
{
    PWR->CR |= PWR_CR_DBP;
    while (!(RTC->CRL & RTC_CRL_RTOFF))
        ;
    RTC->CRL |= RTC_CRL_CNF;

    RTC->CNTH = (time >> 16) & 0xFFFF;
    RTC->CNTL = time & 0xFFFF;

    RTC->CRL &= ~RTC_CRL_CNF;
    while (!(RTC->CRL & RTC_CRL_RTOFF))
        ;
    PWR->CR &= ~PWR_CR_DBP;
}

uint32_t Driver_RTC_GetTime()
{
    uint32_t time = (uint32_t)RTC->CNTL | (RTC->CNTH << 16);
    return time;
}

void Driver_RTC_SetAlarm(uint32_t time)
{
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    RCC->APB1ENR |= RCC_APB1ENR_BKPEN;
    PWR->CR |= PWR_CR_DBP;

    RTC->CRL &= ~RTC_CRL_RSF;
    while (!(RTC->CRL & RTC_CRL_RSF))
        ;
    while (!(RTC->CRL & RTC_CRL_RTOFF))
        ;

    RTC->CRL |= RTC_CRL_CNF;
    RTC->ALRH = (uint16_t)(time >> 16);
    RTC->ALRL = (uint16_t)time;
    RTC->CRL &= ~RTC_CRL_CNF;

    while (!(RTC->CRL & RTC_CRL_RTOFF))
        ;

    PWR->CR &= ~PWR_CR_DBP;
}
