#include "Com_Utils.h"
#include "Driver_Button.h"
#include "Driver_CAN.h"
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
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
        ;
}

// LED
int main(void)
{
    uint16_t id = 0x537;
    uint8_t len = 0;
    uint8_t data[100] = {0};

    STM32_Init();
    Driver_USART_Init();
    Driver_CAN_Init();
    Driver_LED_Init();

    while (1)
    {
        if (Driver_CAN_GetPendingMessageCount() > 0)
        {
            Driver_CAN_Receive(&id, data, &len);

            if (strcmp((char *)data, "on") == 0)
            {
                Driver_LED_On();
            }
            else if (strcmp((char *)data, "off") == 0)
            {
                Driver_LED_Off();
            }
        }
    }
}

// Button
// int main(void)
// {
//     uint16_t id = 0x537;
//     bool led = false;

//     STM32_Init();
//     Driver_USART_Init();
//     Driver_CAN_Init();
//     Driver_Button_Init();

//     while (1)
//     {
//         if (button_flag == true)
//         {
//             button_flag = false;
//             COM_Utils_Delay_ms(10);
//             if ((GPIOA->IDR & BUTTON) != 0)
//             {
//                 if (led)
//                 {
//                     Driver_CAN_Transmit(id, "off", 3);
//                 }
//                 else
//                 {
//                     Driver_CAN_Transmit(id, "on", 2);
//                 }
//                 led = !led;
//                 COM_Utils_Delay_ms(10);
//             }
//         }
//     }
// }