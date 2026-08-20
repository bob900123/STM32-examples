#include "Driver_I2C.h"
#include "stm32f10x.h"

void Driver_I2C_Init(void)
{
    // 時鐘配置
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    // GPIO 工作模式
    GPIOB->CRH |= GPIO_CRH_MODE10;
    GPIOB->CRH |= GPIO_CRH_CNF10;
    GPIOB->CRH |= GPIO_CRH_MODE11;
    GPIOB->CRH |= GPIO_CRH_CNF11;

    // I2C
    I2C2->CR2 |= 36 << 0;
    I2C2->CCR |= 180 << 0;
    I2C2->TRISE |= 37;
    I2C2->CR1 |= I2C_CR1_PE;
}

uint8_t Driver_I2C_Start(void)
{
    I2C2->CR1 |= I2C_CR1_START;

    uint16_t timeout = 0xffff;
    while (((I2C2->SR1 & I2C_SR1_SB) == 0) && timeout)
    {
        timeout--;
    }

    return timeout ? OK : FAIL;
}

void Driver_I2C_Stop(void)
{
    I2C2->CR1 |= I2C_CR1_STOP;
}

void Driver_I2C_Ack(void)
{
    I2C2->CR1 |= I2C_CR1_ACK;
}

void Driver_I2C_NAck(void)
{
    I2C2->CR1 &= ~I2C_CR1_ACK;
}

uint8_t Driver_I2C_SendAddr(uint8_t addr)
{
    I2C2->DR = addr;

    uint16_t timeout = 0xffff;
    while (((I2C2->SR1 & I2C_SR1_ADDR) == 0) && timeout)
    {
        timeout--;
    }

    if (timeout)
    {
        // 清除ADDR，讀SR1 -> 讀SR2
        I2C2->SR1;
        I2C2->SR2;
    }

    return timeout ? OK : FAIL;
}

uint8_t Driver_I2C_SendByte(uint8_t byte)
{
    uint16_t timeout = 0xffff;
    while (((I2C2->SR1 & I2C_SR1_TXE) == 0) && timeout)
    {
        timeout--;
    }

    I2C2->DR = byte;

    timeout = 0xffff;
    while (((I2C2->SR1 & I2C_SR1_BTF) == 0) && timeout)
    {
        timeout--;
    }

    return timeout ? OK : FAIL;
}

uint8_t Driver_I2C_ReadByte(void)
{
    uint16_t timeout = 0xffff;
    while (((I2C2->SR1 & I2C_SR1_RXNE) == 0) && timeout)
    {
        timeout--;
    }
    uint8_t data = timeout ? I2C2->DR : 0;
    return data;
}
