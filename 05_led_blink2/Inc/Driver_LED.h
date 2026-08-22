#ifndef __DRIVER_LED_H__
#define __DRIVER_LED_H__

#include "stm32f10x.h"

#define LED GPIO_ODR_ODR10

void Driver_LED_Init();
void Driver_LED_On();
void Driver_LED_Off();
void Driver_LED_Toggle();

#endif /* __DRIVER_LED_H__ */