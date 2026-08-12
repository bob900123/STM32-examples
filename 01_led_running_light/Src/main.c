#include "Driver_LED.h"
#include <stm32f10x.h>

void delay(unsigned int ms)
{
    unsigned int i;

    while (ms--)
    {
        for (i = 0; i < 8000; i++)
        {
        }
    }
}

uint32_t leds[] = {LED_1, LED_2, LED_3, LED_4, LED_5};

int main(void)
{
    Driver_LED_Init();

    while (1)
    {
        for (int i = 0; i < 5; i++)
        {
            Driver_LED_OffAll(leds, 5);
            Driver_LED_On(leds[i]);
            delay(563);
        }
    }
}
