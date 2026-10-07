#ifndef __DRIVER_HTTP_H__
#define __DRIVER_HTTP_H__

#include "Driver_ESP8266.h"
#include "stm32f10x.h"
#include <stdio.h>

void Driver_HTTP_Init();
bool Driver_HTTP_Post(float value);

#endif /* __DRIVER_HTTP_H__ */