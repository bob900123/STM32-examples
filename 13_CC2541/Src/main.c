#include "Com_Utils.h"
#include "Driver_Bluetooth.h"
#include "Driver_LED.h"
#include "Driver_USART.h"
#include <stdbool.h>
#include <stdio.h>
#include <stm32f10x.h>
#include <string.h>

char usart_buffer[100];
uint16_t usart_len = 0;
uint8_t print_connect_num = 1;
uint8_t print_disconnect_num = 1;

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

void Format_Bluetooth_String(uint8_t *str, uint16_t *len)
{
    uint16_t leds[] = {LED1, LED2, LED3};
    char led_str[3][5];

    for (int i = 0; i < 3; i++)
    {
        if (Driver_LED_Status(leds[i]) == LED_ON)
        {
            strcpy(led_str[i], "ON");
        }
        else
        {
            strcpy(led_str[i], "OFF");
        }
    }

    *len = sprintf((char *)str, "LED:%s,%s,%s", led_str[0], led_str[1], led_str[2]);
}

int main(void)
{
    STM32_Init();
    Driver_USART_Init();
    Driver_Bluetooth_Init();
    Driver_LED_Init();
    Driver_USART_SendString("Success Init\n", 13);

    while (1)
    {
        if (Driver_Bluetooth_Status() == BLUETOOTH_CONNECTED)
        {
            if (print_connect_num)
            {
                Driver_USART_SendString("connected\n", 10);
                print_connect_num--;
            }
            Format_Bluetooth_String((uint8_t *)usart_buffer, &usart_len);
            Driver_Bluetooth_SendString((uint8_t *)usart_buffer, usart_len);

            if (is_bluetooth_receive_finished == true)
            {
                Driver_USART_SendString(bluetooth_buffer, bluetooth_length);
                Driver_USART_SendString("\r\n", 2);
                if (strstr((char *)bluetooth_buffer, "led1") != NULL)
                {
                    Driver_LED_Toggle(LED1);
                }
                else if (strstr((char *)bluetooth_buffer, "led2") != NULL)
                {
                    Driver_LED_Toggle(LED2);
                }
                else if (strstr((char *)bluetooth_buffer, "led3") != NULL)
                {
                    Driver_LED_Toggle(LED3);
                }

                is_bluetooth_receive_finished = false;
                bluetooth_length = 0;
            }
        }
        else
        {
            Driver_USART_SendString("not connected\n", 14);
            print_connect_num = 1;
        }

        Com_Utils_DelayMs(500);
    }
}