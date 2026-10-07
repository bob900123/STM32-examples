#include "Driver_ESP8266.h"
#include "Com_Utils.h"

static uint8_t usart1_dma_buffer[ESP8266_DMA_RX_SIZE];
uint8_t usart1_rx_buffer[ESP8266_RX_BUFFER_SIZE];
volatile uint16_t usart1_rx_length = 0;
volatile bool usart1_is_receive_finished = false;
volatile bool usart1_is_transmit_finished = true;

void Driver_ESP8266_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;

    // PA9 = USART1 TX
    GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
    GPIOA->CRH |= GPIO_CRH_MODE9;
    GPIOA->CRH |= GPIO_CRH_CNF9_1;

    // PA10 = USART1 RX
    GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
    GPIOA->CRH |= GPIO_CRH_CNF10_0;

    // PA11 = ESP8266 RESET
    GPIOA->CRH &= ~(GPIO_CRH_MODE11 | GPIO_CRH_CNF11);
    GPIOA->CRH |= GPIO_CRH_MODE11;

    // ESP8266 Reset
    GPIOA->ODR &= ~GPIO_ODR_ODR11;
    Com_Utils_DelayMs(500);
    GPIOA->ODR |= GPIO_ODR_ODR11;
    Com_Utils_DelayMs(2000);

    USART1->BRR = 0x0271;
    USART1->CR1 |= USART_CR1_TE;
    USART1->CR1 |= USART_CR1_RE;

    USART1->CR3 |= USART_CR3_DMAR;
    USART1->CR3 |= USART_CR3_DMAT;
    USART1->CR1 |= USART_CR1_IDLEIE;
    DMA1_Channel5->CCR &= ~DMA_CCR5_EN;
    DMA1_Channel5->CCR &= ~DMA_CCR5_DIR;
    DMA1_Channel5->CCR |= DMA_CCR5_MINC;
    DMA1_Channel5->CPAR = (uint32_t)&USART1->DR;
    DMA1_Channel5->CMAR = (uint32_t)usart1_dma_buffer;
    DMA1_Channel5->CNDTR = ESP8266_DMA_RX_SIZE;

    DMA1_Channel4->CCR &= ~DMA_CCR4_EN;
    DMA1_Channel4->CCR |= DMA_CCR4_DIR;
    DMA1_Channel4->CCR |= DMA_CCR4_MINC;
    DMA1_Channel4->CCR |= DMA_CCR4_TCIE;

    DMA1_Channel4->CPAR = (uint32_t)&USART1->DR;

    NVIC_SetPriorityGrouping(3);
    NVIC_SetPriority(USART1_IRQn, 2);
    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_SetPriority(DMA1_Channel4_IRQn, 2);
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);

    USART1->CR1 |= USART_CR1_UE;
    DMA1_Channel5->CCR |= DMA_CCR5_EN;

    memset(usart1_dma_buffer, 0, sizeof(usart1_dma_buffer));
    memset(usart1_rx_buffer, 0, sizeof(usart1_rx_buffer));

    usart1_rx_length = 0;
}

void Driver_ESP8266_ClearBuffer(void)
{
    NVIC_DisableIRQ(USART1_IRQn);

    DMA1_Channel5->CCR &= ~DMA_CCR5_EN;

    volatile uint32_t temp;
    temp = USART1->SR;
    temp = USART1->DR;
    (void)temp;

    DMA1->IFCR = DMA_IFCR_CGIF5;

    memset(usart1_dma_buffer, 0, sizeof(usart1_dma_buffer));
    memset(usart1_rx_buffer, 0, sizeof(usart1_rx_buffer));

    usart1_rx_length = 0;
    usart1_is_receive_finished = false;

    DMA1_Channel5->CMAR = (uint32_t)usart1_dma_buffer;
    DMA1_Channel5->CNDTR = ESP8266_DMA_RX_SIZE;

    DMA1_Channel5->CCR |= DMA_CCR5_EN;

    NVIC_ClearPendingIRQ(USART1_IRQn);
    NVIC_EnableIRQ(USART1_IRQn);
}

bool Driver_ESP8266_WaitFor(const char *expected, uint32_t timeout_ms)
{
    while (timeout_ms > 0)
    {
        if (strstr((char *)usart1_rx_buffer, expected) != NULL)
        {
            uint16_t last_len = usart1_rx_length;
            uint32_t quiet = 30;

            while (quiet > 0)
            {
                Com_Utils_DelayMs(1);

                if (usart1_rx_length != last_len)
                {
                    last_len = usart1_rx_length;
                    quiet = 30;
                }
                else
                {
                    quiet--;
                }
            }

            return true;
        }

        if (strstr((char *)usart1_rx_buffer, "ERROR") != NULL)
            return false;
        if (strstr((char *)usart1_rx_buffer, "FAIL") != NULL)
            return false;
        if (strstr((char *)usart1_rx_buffer, "busy") != NULL)
            return false;

        Com_Utils_DelayMs(1);
        timeout_ms--;
    }

    return false;
}

void Driver_ESP8266_SendString(const uint8_t *str, uint16_t len)
{
    if (len == 0)
        return;

    while (!usart1_is_transmit_finished)
        ;

    usart1_is_transmit_finished = false;

    DMA1_Channel4->CCR &= ~DMA_CCR4_EN;
    DMA1->IFCR = DMA_IFCR_CGIF4;

    DMA1_Channel4->CMAR = (uint32_t)str;
    DMA1_Channel4->CNDTR = len;
    DMA1_Channel4->CCR |= DMA_CCR4_EN;

    while (!usart1_is_transmit_finished)
        ;
}

bool Driver_ESP8266_SendCommand(const char *command, const char *expected, uint32_t timeout_ms)
{
    Driver_ESP8266_ClearBuffer();
    Driver_ESP8266_SendString((const uint8_t *)command, strlen(command));

    return Driver_ESP8266_WaitFor(expected, timeout_ms);
}

bool Driver_ESP8266_TCP_Connect(const char *ip, uint16_t port)
{
    char command[100];

    sprintf(command, "AT+CIPSTART=\"TCP\",\"%s\",%u\r\n", ip, port);

    return Driver_ESP8266_SendCommand(command, "CONNECT", 10000);
}

void Driver_ESP8266_TCP_Close(void)
{
    Driver_ESP8266_ClearBuffer();
    Driver_ESP8266_SendString((const uint8_t *)"AT+CIPCLOSE\r\n", 13);

    uint32_t timeout = 2000;
    while (timeout > 0)
    {
        if (strstr((char *)usart1_rx_buffer, "CLOSED") != NULL)
        {
            break;
        }

        if (strstr((char *)usart1_rx_buffer, "OK") != NULL)
        {
            break;
        }

        if (strstr((char *)usart1_rx_buffer, "ERROR") != NULL)
        {
            break;
        }

        Com_Utils_DelayMs(1);
        timeout--;
    }

    Com_Utils_DelayMs(20);
}

bool Driver_ESP8266_TCP_Send(const uint8_t *data, uint16_t len)
{
    char command[50];
    uint32_t timeout;

    sprintf(command, "AT+CIPSEND=%u\r\n", len);

    Driver_ESP8266_ClearBuffer();
    Driver_ESP8266_SendString((const uint8_t *)command, strlen(command));

    timeout = 5000;

    while (timeout > 0)
    {
        if (strstr((char *)usart1_rx_buffer, ">") != NULL)
            break;
        if (strstr((char *)usart1_rx_buffer, "ERROR") != NULL)
            return false;
        if (strstr((char *)usart1_rx_buffer, "FAIL") != NULL)
            return false;
        if (strstr((char *)usart1_rx_buffer, "busy") != NULL)
            return false;
        Com_Utils_DelayMs(1);
        timeout--;
    }

    if (timeout == 0)
        return false;

    Driver_ESP8266_ClearBuffer();
    Driver_ESP8266_SendString(data, len);

    timeout = 10000;
    while (timeout > 0)
    {
        if (strstr((char *)usart1_rx_buffer, "SEND OK") != NULL)
            return true;
        if (strstr((char *)usart1_rx_buffer, "HTTP/1.1") != NULL)
            return true;
        if (strstr((char *)usart1_rx_buffer, "SEND FAIL") != NULL)
            return false;
        if (strstr((char *)usart1_rx_buffer, "ERROR") != NULL)
            return false;
        Com_Utils_DelayMs(1);
        timeout--;
    }

    return false;
}

void USART1_IRQHandler(void)
{
    if (USART1->SR & USART_SR_IDLE)
    {
        volatile uint32_t temp;

        temp = USART1->SR;
        temp = USART1->DR;
        (void)temp;

        DMA1_Channel5->CCR &= ~DMA_CCR5_EN;
        uint16_t len = ESP8266_DMA_RX_SIZE - DMA1_Channel5->CNDTR;

        if (len > 0)
        {
            uint16_t available = (ESP8266_RX_BUFFER_SIZE - 1) - usart1_rx_length;
            if (len > available)
                len = available;

            if (len > 0)
            {
                memcpy(&usart1_rx_buffer[usart1_rx_length], usart1_dma_buffer, len);

                usart1_rx_length += len;
                usart1_rx_buffer[usart1_rx_length] = '\0';
            }
        }

        memset(usart1_dma_buffer, 0, sizeof(usart1_dma_buffer));

        DMA1->IFCR = DMA_IFCR_CGIF5;

        DMA1_Channel5->CMAR = (uint32_t)usart1_dma_buffer;
        DMA1_Channel5->CNDTR = ESP8266_DMA_RX_SIZE;
        DMA1_Channel5->CCR |= DMA_CCR5_EN;

        usart1_is_receive_finished = true;
    }
}

void DMA1_Channel4_IRQHandler(void)
{
    if (DMA1->ISR & DMA_ISR_TCIF4)
    {
        DMA1->IFCR = DMA_IFCR_CGIF4;
        DMA1_Channel4->CCR &= ~DMA_CCR4_EN;
        usart1_is_transmit_finished = true;
    }
}