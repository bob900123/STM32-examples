#ifndef __DRIVER_BKP_H__
#define __DRIVER_BKP_H__

#include "stm32f10x.h"

void Driver_BKP_Write(uint8_t *str, uint8_t length);
void Driver_BKP_Read(uint8_t *str, uint8_t length);

#endif /* __DRIVER_BKP_H__ */