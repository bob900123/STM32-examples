#include <stm32f10x.h>

int main(void)
{
    // 時鐘配置
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // GPIO 工作模式
    GPIOA->CRL &= ~GPIO_CRL_CNF0;
    GPIOA->CRL |= GPIO_CRL_MODE0;

    // PA0 輸出高電平
    GPIOA->ODR |= GPIO_ODR_ODR0;

    while (1)
    {
        // code here
    }
}