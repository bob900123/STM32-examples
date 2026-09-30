#ifndef __DRIVER_W5500_H__
#define __DRIVER_W5500_H__

#include "stm32f10x.h"
#include "Com_Utils.h"
#include "Driver_SPI.h"
#include "Driver_USART.h"
#include "w5500.h"
#include "wizchip_conf.h"
#include <string.h>
#include <stdio.h>

void Driver_W5500_Init();

#endif /* __DRIVER_W5500_H__ */