#ifndef __DRIVER_USART_H__
#define __DRIVER_USART_H__

#include "stm32f10x.h"
#include <stdbool.h>

extern bool is_finish;
extern uint8_t length;
extern uint8_t buffer[100];

void Driver_USART_Init();

void Driver_USART_SendChar(uint8_t byte);

void Driver_USART_SendString(uint8_t *str, uint16_t len);

#endif /* __DRIVER_USART_H__ */