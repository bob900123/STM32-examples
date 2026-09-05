#include "Driver_SPI.h"
#include "Driver_USART.h"

void Driver_SPI_Init()
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // spi
    GPIOA->CRL |= GPIO_CRL_MODE5;
    GPIOA->CRL &= ~GPIO_CRL_CNF5;
    GPIOA->CRL |= GPIO_CRL_CNF5_1;

    GPIOA->CRL &= ~GPIO_CRL_MODE6;
    GPIOA->CRL &= ~GPIO_CRL_CNF6;
    GPIOA->CRL |= GPIO_CRL_CNF6_0;

    GPIOA->CRL |= GPIO_CRL_MODE7;
    GPIOA->CRL &= ~GPIO_CRL_CNF7;
    GPIOA->CRL |= GPIO_CRL_CNF7_1;

    // cs
    GPIOB->CRL |= GPIO_CRL_MODE0;
    GPIOB->CRL &= ~GPIO_CRL_CNF0;
    CS_HIGH;

    // MODE 0
    SPI1->CR1 &= ~SPI_CR1_CPHA;
    SPI1->CR1 &= ~SPI_CR1_CPOL;

    SPI1->CR1 |= SPI_CR1_MSTR;

    SPI1->CR1 &= ~SPI_CR1_BR;
    SPI1->CR1 |= SPI_CR1_BR_1;

    SPI1->CR1 &= ~SPI_CR1_LSBFIRST;
    SPI1->CR1 |= SPI_CR1_SSI;
    SPI1->CR1 |= SPI_CR1_SSM;

    SPI1->CR1 |= SPI_CR1_SPE;
}

void Driver_SPI_Start(void)
{
    CS_LOW;
}

void Driver_SPI_Stop(void)
{
    CS_HIGH;
}

uint8_t Driver_SPI_SwapByte(uint8_t byte)
{
    while (!(SPI1->SR & SPI_SR_TXE))
        ;

    SPI1->DR = byte;

    while (!(SPI1->SR & SPI_SR_RXNE))
        ;

    return (uint8_t)(SPI1->DR & 0x00ff);
}