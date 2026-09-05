#ifndef __DRIVER_SPI_H__
#define __DRIVER_SPI_H__

#include "stm32f10x.h"

#define OK      1
#define FAIL    0

#define CS_HIGH (GPIOB->ODR |= GPIO_ODR_ODR0)
#define CS_LOW  (GPIOB->ODR &= ~GPIO_ODR_ODR0)

void Driver_SPI_Init();
void Driver_SPI_Start(void);
void Driver_SPI_Stop(void);
uint8_t Driver_SPI_SwapByte(uint8_t byte);

#endif /* __DRIVER_SPI_H__ */