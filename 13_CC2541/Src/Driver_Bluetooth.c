#include "Driver_Bluetooth.h"
#include "Com_Utils.h"
#include "Driver_USART.h"

bool is_bluetooth_receive_finished = false;
bool is_bluetooth_transmit_finished = true;
uint8_t bluetooth_length = 0;
uint8_t bluetooth_buffer[101] = {0};

void Driver_Bluetooth_Init()
{
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    // RXD - PA3
    GPIOA->CRL &= ~GPIO_CRL_MODE3;
    GPIOA->CRL |= GPIO_CRL_CNF3_0;
    GPIOA->CRL &= ~GPIO_CRL_CNF3_1;

    // TXD - PA2
    GPIOA->CRL |= GPIO_CRL_MODE2;
    GPIOA->CRL |= GPIO_CRL_CNF2_1;
    GPIOA->CRL &= ~GPIO_CRL_CNF2_0;

    // Status - PB0
    GPIOB->CRL &= ~GPIO_CRL_MODE0;
    GPIOB->CRL |= GPIO_CRL_CNF0_0;
    GPIOB->CRL &= ~GPIO_CRL_CNF0_1;

    // baud rate (36 MHz) - 9600
    USART2->BRR = 0x0ea6;
    USART2->CR1 |= (USART_CR1_TE | USART_CR1_RE);
    USART2->CR3 |= USART_CR3_DMAR;
    USART2->CR3 |= USART_CR3_DMAT;
    USART2->CR1 |= USART_CR1_IDLEIE;

    // USART2 RX use DMA
    DMA1_Channel6->CCR &= ~DMA_CCR6_DIR;
    DMA1_Channel6->CCR |= DMA_CCR6_MINC;
    DMA1_Channel6->CNDTR = 100;

    DMA1_Channel6->CPAR = (uint32_t)&(USART2->DR);
    DMA1_Channel6->CMAR = (uint32_t)bluetooth_buffer;

    // USART2 TX use DMA
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

    Com_Utils_DelayS(2);
    Driver_USART_SendString("Send At+RESET\n", 14);
    Driver_Bluetooth_SendString("AT+RESET\r\n", 10);
    // Com_Utils_DelayS(1);
    while (strstr((char *)bluetooth_buffer, "OK") == NULL && bluetooth_length != 0)
        ;
}

void Driver_Bluetooth_SendString(uint8_t *str, uint16_t len)
{
    while (!is_bluetooth_transmit_finished)
        ;
    is_bluetooth_transmit_finished = false;
    DMA1_Channel7->CCR &= ~DMA_CCR7_EN;
    DMA1->IFCR = DMA_IFCR_CGIF7;
    DMA1_Channel7->CNDTR = (uint32_t)len;
    DMA1_Channel7->CMAR = (uint32_t)str;
    DMA1_Channel7->CCR |= DMA_CCR7_EN;
}

uint8_t Driver_Bluetooth_Status()
{
    if ((GPIOB->IDR & GPIO_IDR_IDR0) == 0)
    {
        return BLUETOOTH_DISCONNECTED;
    }

    return BLUETOOTH_CONNECTED;
}

void USART2_IRQHandler()
{
    if (USART2->SR & USART_SR_IDLE)
    {
        // 重置 idle
        USART2->SR;
        USART2->DR;
        DMA1_Channel6->CCR &= ~DMA_CCR6_EN;

        bluetooth_length = 100 - DMA1_Channel6->CNDTR;
        bluetooth_buffer[bluetooth_length] = '\0';
        DMA1_Channel6->CNDTR = 100;
        is_bluetooth_receive_finished = true;
        DMA1_Channel6->CCR |= DMA_CCR6_EN;
    }
}

void DMA1_Channel7_IRQHandler(void)
{
    if (DMA1->ISR & DMA_ISR_TCIF7)
    {
        DMA1->IFCR |= DMA_IFCR_CGIF7;
        DMA1_Channel7->CCR &= ~DMA_CCR7_EN;
        is_bluetooth_transmit_finished = true;
    }
}