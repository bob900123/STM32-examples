#ifndef __DRIVER_USART_H__
#define __DRIVER_USART_H__

#include "stm32f10x.h"
#include <stdbool.h>

extern volatile bool usart2_is_receive_finished;
extern volatile uint8_t usart2_length;
extern volatile uint8_t usart2_rx_buffer[100];

void Driver_USART_Init();
void Driver_USART_SendString(uint8_t *str, uint16_t len);

#endif /* __DRIVER_USART_H__ */