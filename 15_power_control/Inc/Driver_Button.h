#ifndef __DRIVER_BUTTON_H__
#define __DRIVER_BUTTON_H__

#include <stm32f10x.h>
#include "Com_Utils.h"

#define BUTTON GPIO_IDR_IDR2

void Driver_Button_Init();

#endif /* __DRIVER_BUTTON_H__ */