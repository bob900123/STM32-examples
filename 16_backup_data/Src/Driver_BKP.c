#include "Driver_BKP.h"

void Driver_BKP_Write(uint8_t *str, uint8_t length)
{
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    RCC->APB1ENR |= RCC_APB1ENR_BKPEN;

    length = length > 20 ? 20 : length;

    PWR->CR |= PWR_CR_DBP;

    for (uint8_t i = 0; i < length; i += 2)
    {
        uint16_t data = str[i];

        if (i + 1 < length)
        {
            data |= (uint16_t)str[i + 1] << 8;
        }

        (&BKP->DR1)[i] = data;
    }

    PWR->CR &= ~PWR_CR_DBP;
}

void Driver_BKP_Read(uint8_t *str, uint8_t length)
{
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    RCC->APB1ENR |= RCC_APB1ENR_BKPEN;

    length = length > 20 ? 20 : length;

    for (uint8_t i = 0; i < length; i += 2)
    {
        uint16_t data = (&BKP->DR1)[i];
        str[i] = (uint8_t)data;

        if (i + 1 < length)
        {
            str[i + 1] = (uint8_t)(data >> 8);
        }
    }
}
