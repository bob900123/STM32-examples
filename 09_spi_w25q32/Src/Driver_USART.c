#include "Driver_USART.h"
#include "stm32f10x.h"

bool is_receive_finished = false;
bool is_transmit_finished = true;
uint8_t length = 0;
uint8_t buffer[100] = {0};

void Driver_USART_Init(uint32_t destAddr)
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
    USART1->CR3 |= USART_CR3_DMAR;
    USART1->CR3 |= USART_CR3_DMAT;
    USART1->CR1 |= USART_CR1_IDLEIE;

    // USART1 RX use DMA
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    DMA1_Channel5->CCR &= ~DMA_CCR5_DIR;
    DMA1_Channel5->CCR |= DMA_CCR5_MINC;
    DMA1_Channel5->CNDTR = 100;

    DMA1_Channel5->CPAR = (uint32_t)&(USART1->DR);
    DMA1_Channel5->CMAR = destAddr;

    // USART1 TX use DMA
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    DMA1_Channel4->CCR |= DMA_CCR4_DIR;
    DMA1_Channel4->CCR |= DMA_CCR4_MINC;
    DMA1_Channel4->CCR |= DMA_CCR4_TCIE;

    DMA1_Channel4->CPAR = (uint32_t)&(USART1->DR);

    NVIC_SetPriorityGrouping(3);
    NVIC_SetPriority(USART1_IRQn, 2);
    NVIC_EnableIRQ(USART1_IRQn);

    NVIC_SetPriority(DMA1_Channel4_IRQn, 2);
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);

    USART1->CR1 |= USART_CR1_UE;
    DMA1_Channel5->CCR |= DMA_CCR5_EN;
}

void Driver_USART_SendString(uint8_t *str, uint16_t len)
{
    while (!is_transmit_finished)
        ;
    is_transmit_finished = false;
    DMA1_Channel4->CNDTR = (uint32_t)len;
    DMA1_Channel4->CMAR = (uint32_t)str;
    DMA1_Channel4->CCR |= DMA_CCR4_EN;
}

void USART1_IRQHandler()
{
    if (USART1->SR & USART_SR_IDLE)
    {
        // 重置 idle
        USART1->SR;
        USART1->DR;
        length = 100 - DMA1_Channel5->CNDTR;
        buffer[length] = '\0';
        is_receive_finished = true;

        DMA1_Channel5->CCR &= ~DMA_CCR5_EN;
        DMA1_Channel5->CNDTR = 100;
        DMA1_Channel5->CCR |= DMA_CCR5_EN;

        return;
    }
}

void DMA1_Channel4_IRQHandler(void)
{
    if (DMA1->ISR & DMA_ISR_TCIF4)
    {
        DMA1->IFCR |= DMA_IFCR_CGIF4;
        DMA1_Channel4->CCR &= ~DMA_CCR4_EN;
        is_transmit_finished = true;
    }
}
