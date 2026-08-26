#ifndef __DRIVER_LED_H__
#define __DRIVER_LED_H__

#include "stm32f10x.h"

#define LED GPIO_ODR_ODR6

void Driver_LED_Init();
void Driver_LED_Brightness(uint8_t brightness);

#endif /* __DRIVER_LED_H__ */