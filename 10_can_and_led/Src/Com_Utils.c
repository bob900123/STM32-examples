#include "Com_Utils.h"
#include <stm32f10x.h>

void COM_Utils_Delay_us(uint16_t us)
{
    SysTick->LOAD = 72 * us;
    SysTick->VAL = 0;
    SysTick->CTRL = 0x05;
    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG))
        ;
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE;
}

void COM_Utils_Delay_ms(uint16_t ms)
{
    while (ms--)
    {
        COM_Utils_Delay_us(1000);
    }
}

void COM_Utils_Delay_s(uint16_t s)
{
    while (s--)
    {
        COM_Utils_Delay_ms(1000);
    }
}