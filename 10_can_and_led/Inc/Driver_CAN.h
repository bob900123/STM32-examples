#ifndef __DRIVER_CAN_H__
#define __DRIVER_CAN_H__

#include <stm32f10x.h>
#include <stdbool.h>

void Driver_CAN_Init();
bool Driver_CAN_Transmit(uint16_t id, uint8_t *data, uint8_t len);
bool Driver_CAN_Receive(uint16_t *id, uint8_t *data, uint8_t *len);
uint8_t Driver_CAN_GetPendingMessageCount();

#endif /* __DRIVER_CAN_H__ */