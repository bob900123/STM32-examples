#ifndef __COM_UTILS_H__
#define __COM_UTILS_H__

#include <stm32f10x.h>

void Com_Utils_Delay(uint32_t count);
uint8_t Com_Utils_GetLength(char *buffer, uint8_t length);

#endif /* __COM_UTILS_H__ */