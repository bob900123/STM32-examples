#include "Com_utils.h"
#include "Driver_Button.h"
#include "Driver_LED.h"
#include <stdbool.h>
#include <stm32f10x.h>

int main(void)
{
    Driver_Button_Init();
    Driver_LED_Init();

    while (1)
    {
        if (button_flag == true)
        {
            button_flag = false;
            delay(100);
            if ((GPIOA->IDR & BUTTON) != 0)
            {
                Driver_LED_Toggle();
                while ((GPIOA->IDR & BUTTON) != 0)
                    ;
                delay(100);
            }
        }
    }
}