#ifndef __DRIVER_LED_H__
#define __DRIVER_LED_H__

#include "stm32f10x.h"

void Driver_LED_Init(void);
void Driver_LED_On(void);
void Driver_LED_Off(void);
void Driver_LED_SetBrightness(uint8_t brightness);

#endif /* __DRIVER_LED_H__ */
