#ifndef __DRIVER_BLUETOOTH_H__
#define __DRIVER_BLUETOOTH_H__

#include "stm32f10x.h"
#include "string.h"
#include "stdio.h"
#include "stdbool.h"

#define BLUETOOTH_DISCONNECTED 0
#define BLUETOOTH_CONNECTED    1

extern bool is_bluetooth_receive_finished;
extern uint8_t bluetooth_length;
extern uint8_t bluetooth_buffer[101];

void Driver_Bluetooth_Init();
uint8_t Driver_Bluetooth_Status();
void Driver_Bluetooth_SendString();

#endif /* __DRIVER_BLUETOOTH_H__ */