#include "Driver_IWDG.h"

void Driver_IWDG_Init()
{
    IWDG->KR = 0xCCCC;
    IWDG->KR = 0x5555;

    while (IWDG->SR != 0)
        ;

    IWDG->PR = 4;
    IWDG->RLR = 3124;

    while (IWDG->SR != 0)
        ;

    // 1 ÷ 40000 x 64 x (3124 + 1) = 5

    Driver_IWDG_Refresh();
}

void Driver_IWDG_Refresh()
{
    IWDG->KR = 0xAAAA;
}
