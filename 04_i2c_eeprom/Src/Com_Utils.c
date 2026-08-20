#include <stm32f10x.h>

void Com_Utils_Delay(uint32_t count)
{
    uint32_t i;

    while (count--)
    {
        for (i = 0; i < 8000; i++)
        {
        }
    }
}

uint8_t Com_Utils_GetLength(char *buffer, uint8_t length)
{
    uint8_t count = 0;

    for (int i = 0; i < length; i++)
    {
        if (buffer[i] <= 57 && buffer[i] >= 48)
        {
            count = count * 10 + buffer[i] - 48;
        }
    }
    return count;
}