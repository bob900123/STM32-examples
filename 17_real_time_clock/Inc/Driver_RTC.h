#ifndef __DRIVER_RTC_H__
#define __DRIVER_RTC_H__

#include "stm32f10x.h"
#include "Driver_USART.h"

void Driver_RTC_Init();
void Driver_RTC_SetTime(uint32_t time);
void Driver_RTC_SetAlarm(uint32_t time);
uint32_t Driver_RTC_GetTime();

#endif /* __DRIVER_RTC_H__ */