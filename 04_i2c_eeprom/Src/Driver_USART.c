#include "Driver_USART.h"
#include "stm32f10x.h"

bool is_finish = false;
uint8_t length = 0;
uint8_t buffer[100] = {0};

void Driver_USART_Init()
{
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    GPIOA->CRH |= GPIO_CRH_MODE9;
    GPIOA->CRH |= GPIO_CRH_CNF9_1;
    GPIOA->CRH &= ~GPIO_CRH_CNF9_0;

    GPIOA->CRH &= ~GPIO_CRH_MODE10;
    GPIOA->CRH |= GPIO_CRH_CNF10_0;
    GPIOA->CRH &= ~GPIO_CRH_CNF10_1;

    // baud rate
    USART1->BRR = 0x1d4c;
    USART1->CR1 |= (USART_CR1_TE | USART_CR1_RE);
    USART1->CR1 |= USART_CR1_RXNEIE;
    USART1->CR1 |= USART_CR1_IDLEIE;

    USART1->CR1 |= USART_CR1_UE;

    NVIC_SetPriorityGrouping(3);
    NVIC_SetPriority(USART1_IRQn, 2);
    NVIC_EnableIRQ(USART1_IRQn);
}

void Driver_USART_SendChar(uint8_t byte)
{
    while ((USART1->SR & USART_SR_TXE) == 0)
        ;
    USART1->DR = byte;
}

void Driver_USART_SendString(uint8_t *str, uint16_t len)
{
    for (int i = 0; i < len; i++)
    {
        Driver_USART_SendChar(str[i]);
    }
}

void USART1_IRQHandler()
{
    if (USART1->SR & USART_SR_RXNE)
    {
        buffer[length] = USART1->DR;
        length++;
    }
    else if (USART1->SR & USART_SR_IDLE)
    {
        USART1->SR;
        USART1->DR;
        buffer[length] = '\0';
        is_finish = true;
        return;
    }
}