#include "Com_Delay.h"

void Com_DelayUs(uint16_t us)
{
    SysTick->LOAD = 9 * us;
    SysTick->VAL = 0;
    SysTick->CTRL = 0x01;
    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG))
        ;
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE;
}

void Com_DelayMs(uint16_t ms)
{
    while (ms--)
    {
        Com_DelayUs(1000);
    }
}

void Com_DelayS(uint16_t s)
{
    while (s--)
    {
        Com_DelayMs(1000);
    }
}