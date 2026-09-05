#ifndef __DRIVER_FLASH_H__
#define __DRIVER_FLASH_H__

#include "Driver_SPI.h"

#define SECTOR_SIZE 4096

void Driver_Flash_Init();
void Driver_Flash_ReadId(uint8_t *mid, uint8_t *did);
void Driver_Flash_WaitNotBusy(void);
void Driver_Flash_Read(uint8_t block, uint8_t sector, uint8_t page, uint8_t *data, uint16_t len);
void Driver_Flash_WritePage(uint8_t block, uint8_t sector, uint8_t page, uint8_t *data, uint16_t len);

#endif /* __DRIVER_FLASH_H__ */