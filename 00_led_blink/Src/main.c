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

int main(void)
{
    // 時鐘配置
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // 模式
    GPIOA->CRL |= GPIO_CRL_MODE2;
    GPIOA->CRL &= ~GPIO_CRL_CNF2;

    // 輸出
    GPIOA->ODR |= GPIO_ODR_ODR2;

    while (1)
    {
        GPIOA->ODR ^= GPIO_ODR_ODR2;
        delay(1126);
    }
}