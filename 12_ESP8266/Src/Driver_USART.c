#include "Driver_USART.h"

volatile bool usart2_is_receive_finished = false;
volatile bool usart2_is_transmit_finished = true;
volatile uint8_t usart2_length = 0;
volatile uint8_t usart2_rx_buffer[100] = {0};

void Driver_USART_Init()
{
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // PA2 TX
    GPIOA->CRL |= GPIO_CRL_MODE2;
    GPIOA->CRL |= GPIO_CRL_CNF2_1;
    GPIOA->CRL &= ~GPIO_CRL_CNF2_0;

    // PA3 RX
    GPIOA->CRL &= ~GPIO_CRL_MODE3;
    GPIOA->CRL |= GPIO_CRL_CNF3_0;
    GPIOA->CRL &= ~GPIO_CRL_CNF3_1;

    // baud rate
    USART2->BRR = 0x0ea6;
    USART2->CR1 |= (USART_CR1_TE | USART_CR1_RE);
    USART2->CR3 |= USART_CR3_DMAR;
    USART2->CR3 |= USART_CR3_DMAT;
    USART2->CR1 |= USART_CR1_IDLEIE;

    // USART2 RX use DMA
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    DMA1_Channel6->CCR &= ~DMA_CCR6_DIR;
    DMA1_Channel6->CCR |= DMA_CCR6_MINC;
    DMA1_Channel6->CNDTR = 100;

    DMA1_Channel6->CPAR = (uint32_t)&(USART2->DR);
    DMA1_Channel6->CMAR = (uint32_t)usart2_rx_buffer;

    // USART2 TX use DMA
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    DMA1_Channel7->CCR |= DMA_CCR7_DIR;
    DMA1_Channel7->CCR |= DMA_CCR7_MINC;
    DMA1_Channel7->CCR |= DMA_CCR7_TCIE;

    DMA1_Channel7->CPAR = (uint32_t)&(USART2->DR);

    NVIC_SetPriorityGrouping(3);
    NVIC_SetPriority(USART2_IRQn, 2);
    NVIC_EnableIRQ(USART2_IRQn);

    NVIC_SetPriority(DMA1_Channel7_IRQn, 2);
    NVIC_EnableIRQ(DMA1_Channel7_IRQn);

    USART2->CR1 |= USART_CR1_UE;
    DMA1_Channel6->CCR |= DMA_CCR6_EN;
}

void Driver_USART_SendString(uint8_t *str, uint16_t len)
{
    if (len == 0)
        return;

    while (!usart2_is_transmit_finished)
        ;

    usart2_is_transmit_finished = false;

    DMA1_Channel7->CCR &= ~DMA_CCR7_EN;

    DMA1_Channel7->CNDTR = len;
    DMA1_Channel7->CMAR = (uint32_t)str;

    DMA1_Channel7->CCR |= DMA_CCR7_EN;

    // 等這一次 DMA 傳送完成
    while (!usart2_is_transmit_finished)
        ;
}

void USART2_IRQHandler()
{
    if (USART2->SR & USART_SR_IDLE)
    {
        // 重置 idle
        USART2->SR;
        USART2->DR;
        usart2_length = 100 - DMA1_Channel6->CNDTR;
        usart2_rx_buffer[usart2_length] = '\0';

        DMA1_Channel6->CCR &= ~DMA_CCR6_EN;
        DMA1_Channel6->CNDTR = 100;
        DMA1_Channel6->CCR |= DMA_CCR6_EN;

        usart2_is_receive_finished = true;
        return;
    }
}

void DMA1_Channel7_IRQHandler(void)
{
    if (DMA1->ISR & DMA_ISR_TCIF7)
    {
        DMA1->IFCR |= DMA_IFCR_CGIF7;
        DMA1_Channel7->CCR &= ~DMA_CCR7_EN;
        usart2_is_transmit_finished = true;
    }
}
