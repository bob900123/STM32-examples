#include "Com_Utils.h"
#include "Driver_Button.h"
#include "Driver_LED.h"
#include "Driver_USART.h"
#include <stdbool.h>
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

void enter_sleep_mode(void)
{
    // 任一中斷可喚醒
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP;

    // 進入睡眠模式
    __WFI();
}

void enter_stop_mode(void)
{
    // 外部中斷才可喚醒
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR &= ~PWR_CR_PDDS;
    PWR->CR |= PWR_CR_LPDS;
    SCB->SCR |= SCB_SCR_SLEEPDEEP;

    // 進入停止模式
    __WFI();
}

void enter_standby_mode(void)
{
    // 使用 PA0 上升緣喚醒
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CSR |= PWR_CSR_EWUP;
    PWR->CR |= PWR_CR_CWUF;
    while (PWR->CSR & PWR_CSR_WUF)
        ;
    PWR->CR |= PWR_CR_PDDS;
    SCB->SCR |= SCB_SCR_SLEEPDEEP;

    // 進入待機模式
    __WFI();
}

// 睡眠模式
// int main(void)
// {
//     STM32_Init();
//     Driver_USART_Init();
//     Driver_LED_Init();
//     Driver_USART_SendString("Init Success\n", 13);

//     Driver_LED_On();
//     Driver_USART_SendString("Open LED\n", 9);
//     Com_Utils_DelayS(3);

//     char usart_buffer[10];
//     uint8_t usart_len = 0;

//     while (1)
//     {
//         Driver_USART_SendString("Enter sleep mode after 3 seconds\r\n", 34);
//         for (uint8_t i = 0; i < 3; i++)
//         {
//             usart_len = sprintf(usart_buffer, "%d...\r\n", 3 - i);
//             Driver_USART_SendString((uint8_t *)usart_buffer, usart_len);
//             Com_Utils_DelayS(1);
//         }
//         Driver_USART_SendString("Sleeping...\r\n", 13);
//         Com_Utils_DelayS(1);
//         enter_sleep_mode();

//         // GPIO ON

//         Driver_USART_SendString("Waking Up...\r\n", 14);
//     }
// }

// 停止模式
// int main(void)
// {
//     STM32_Init();
//     Driver_USART_Init();
//     Driver_LED_Init();
//     Driver_Button_Init();
//     Driver_USART_SendString("Init Success\n", 13);

//     Driver_LED_On();
//     Driver_USART_SendString("Open LED\n", 9);
//     Com_Utils_DelayS(3);

//     char usart_buffer[10];
//     uint8_t usart_len = 0;

//     while (1)
//     {
//         Driver_USART_SendString("Enter stop mode after 3 seconds\r\n", 33);
//         for (uint8_t i = 0; i < 3; i++)
//         {
//             usart_len = sprintf(usart_buffer, "%d...\r\n", 3 - i);
//             Driver_USART_SendString((uint8_t *)usart_buffer, usart_len);
//             Com_Utils_DelayS(1);
//         }
//         Driver_USART_SendString("Stopping...\r\n", 13);
//         Com_Utils_DelayS(1);
//         enter_stop_mode();

//         // GPIO ON

//         // 恢復後，預設為 HSI，所以需要切換到 HSE
//         STM32_Init();

//         Com_Utils_DelayMs(100);
//         Driver_USART_SendString("Waking Up...\r\n", 14);
//     }
// }

// 待機模式
int main(void)
{
    STM32_Init();
    Driver_USART_Init();
    Driver_LED_Init();
    Driver_Button_Init();
    Driver_USART_SendString("Init Success\n", 13);

    Driver_LED_On();
    Driver_USART_SendString("Open LED\n", 9);
    Com_Utils_DelayS(3);

    char usart_buffer[20];
    uint8_t usart_len = 0;

    while (1)
    {
        Driver_USART_SendString("Enter standby mode after 3 seconds\r\n", 36);
        for (uint8_t i = 0; i < 3; i++)
        {
            usart_len = sprintf(usart_buffer, "%d...\r\n", 3 - i);
            Driver_USART_SendString((uint8_t *)usart_buffer, usart_len);
            Com_Utils_DelayS(1);
        }
        Driver_USART_SendString("Sleeping...\r\n", 13);
        Com_Utils_DelayS(1);
        enter_standby_mode();

        // GPIO OFF
    }
}