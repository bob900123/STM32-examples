#include "Com_Utils.h"

void Com_Utils_DelayUs(uint16_t us)
{
    SysTick->LOAD = 72 * us;
    SysTick->VAL = 0;
    SysTick->CTRL = 0x05; // 不使用中斷
    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG))
        ;
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE;
}

void Com_Utils_DelayMs(uint16_t ms)
{
    while (ms--)
    {
        Com_Utils_DelayUs(1000);
    }
}

void Com_Utils_DelayS(uint16_t s)
{
    while (s--)
    {
        Com_Utils_DelayMs(1000);
    }
}