#include "Com_Utils.h"
#include "Driver_RTC.h"
#include "Driver_USART.h"
#include <stdio.h>
#include <stm32f10x.h>
#include <string.h>
#include <time.h>

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

void enter_standby_mode(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_CSBF;
    while (PWR->CSR & PWR_CSR_SBF)
        ;
    PWR->CR |= PWR_CR_CWUF;
    while (PWR->CSR & PWR_CSR_WUF)
        ;
    PWR->CR |= PWR_CR_PDDS;
    SCB->SCR |= SCB_SCR_SLEEPDEEP;

    // 進入待機模式
    __WFI();
}

int format_time(char *buf, const struct tm *t)
{
    if (buf == NULL || t == NULL)
        return -1;

    return sprintf(buf, "%04d/%02d/%02d %02d:%02d:%02d\r\n",
                   t->tm_year + 1900,
                   t->tm_mon + 1,
                   t->tm_mday,
                   t->tm_hour,
                   t->tm_min,
                   t->tm_sec);
}

// 待機模式
int main(void)
{
    STM32_Init();
    Driver_USART_Init();

    char usart_buffer[50];
    uint8_t usart_len = 0;
    uint32_t second = 5 - 1;
    uint32_t time = 1793662245;

    if (BKP->DR1 != 0xA5A5)
    {
        Driver_RTC_Init();
        Driver_USART_SendString("RTC Init\r\n", 10);
        Driver_RTC_SetTime(time);

        PWR->CR |= PWR_CR_DBP;
        BKP->DR1 = 0xA5A5;
        PWR->CR &= ~PWR_CR_DBP;
    }
    else
    {
        Driver_USART_SendString("RTC Used\r\n", 10);
    }

    while (1)
    {
        uint32_t time = Driver_RTC_GetTime();
        struct tm *timeinfo = localtime(&time);
        usart_len = format_time(usart_buffer, timeinfo);
        Driver_USART_SendString((uint8_t *)usart_buffer, usart_len);
        Com_Utils_DelayMs(50);
        time = Driver_RTC_GetTime() + second;

        Driver_RTC_SetAlarm(time);

        enter_standby_mode();
    }
}
