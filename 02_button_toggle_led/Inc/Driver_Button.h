#ifndef __DRIVER_BUTTON_H__
#define __DRIVER_BUTTON_H__

#include <stdbool.h>
#include <stm32f10x.h>

#define BUTTON GPIO_IDR_IDR2

void Driver_Button_Init();

extern bool button_flag;

#endif /* __DRIVER_BUTTON_H__ */