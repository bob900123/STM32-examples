#ifndef __DRIVER_I2C_H__
#define __DRIVER_I2C_H__

#include "stm32f10x.h"

#define ACK  0
#define NACK 1

#define OK   1
#define FAIL 0

void Driver_I2C_Init(void);

uint8_t Driver_I2C_Start(void);

void Driver_I2C_Stop(void);

void Driver_I2C_Ack(void);

void Driver_I2C_NAck(void);

uint8_t Driver_I2C_SendAddr(uint8_t addr);

uint8_t Driver_I2C_SendByte(uint8_t byte);

uint8_t Driver_I2C_ReadByte(void);

#endif /* __DRIVER_I2C_H__ */