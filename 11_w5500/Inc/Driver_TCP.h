#ifndef __DRIVER_TCP_H__
#define __DRIVER_TCP_H__

#include "Driver_W5500.h"
#include "Driver_USART.h"
#include "socket.h"
#include <stm32f10x.h>

#define SN 0

void Driver_TCP_ServerStart(void);
void Driver_TCP_ClientStart(void);
void Driver_TCP_RecvData(uint8_t buff[], uint16_t *len);
void Driver_TCP_SendData(uint8_t data[], uint16_t len);

#endif /* __DRIVER_TCP_H__ */