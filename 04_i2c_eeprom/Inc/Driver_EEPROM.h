#ifndef __DRIVER_EEPROM_H__
#define __DRIVER_EEPROM_H__

void Driver_EEPROM_Init();
uint8_t Driver_EEPROM_WritePage(uint8_t addr, uint8_t *bytes, uint8_t len);
uint8_t Driver_EEPROM_WriteBytes(uint8_t addr, uint8_t *bytes, uint8_t len);
uint8_t Driver_EEPROM_ReadBytes(uint8_t addr, uint8_t *bytes, uint8_t len);

#endif /* __DRIVER_EEPROM_H__ */