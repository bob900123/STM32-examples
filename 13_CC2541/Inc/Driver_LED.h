#ifndef __DRIVER_LED_H__
#define __DRIVER_LED_H__

#include "stm32f10x.h"

#define LED1    GPIO_ODR_ODR5
#define LED2    GPIO_ODR_ODR6
#define LED3    GPIO_ODR_ODR7

#define LED_OFF 0
#define LED_ON  1

void Driver_LED_Init();
void Driver_LED_On(uint16_t LED);
void Driver_LED_Off(uint16_t LED);
void Driver_LED_Toggle(uint16_t LED);
uint8_t Driver_LED_Status(uint16_t LED);

#endif /* __DRIVER_LED_H__ */