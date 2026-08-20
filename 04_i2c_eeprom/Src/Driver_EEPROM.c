#include "Com_Utils.h"
#include "Driver_I2C.h"
#include "stm32f10x.h"
#include <stdio.h>

#define EEPROM_ADDRESS 0xA0
#define PAGE_SIZE 8

void Driver_EEPROM_Init()
{
    Driver_I2C_Init();
}

uint8_t Driver_EEPROM_WritePage(uint8_t addr, uint8_t *bytes, uint8_t len)
{
    uint8_t ack = OK;

    ack &= Driver_I2C_Start();
    ack &= Driver_I2C_SendAddr(EEPROM_ADDRESS);
    ack &= Driver_I2C_SendByte(addr);
    for (int i = 0; i < len; i++)
    {
        ack &= Driver_I2C_SendByte(bytes[i]);
    }
    Driver_I2C_Stop();
    Com_Utils_Delay(20);

    return ack;
}

uint8_t Driver_EEPROM_WriteBytes(uint8_t addr, uint8_t *bytes, uint8_t len)
{
    unsigned char page_remain;
    uint8_t ack = OK;

    while (len > 0)
    {
        page_remain = PAGE_SIZE - (addr % PAGE_SIZE);

        if (len > page_remain)
        {
            ack &= Driver_EEPROM_WritePage(addr, bytes, page_remain);
            len -= page_remain;
            bytes += page_remain;
            addr += page_remain;
        }
        else
        {
            ack &= Driver_EEPROM_WritePage(addr, bytes, len);
            len = 0;
        }
    }

    return ack;
}

uint8_t Driver_EEPROM_ReadBytes(uint8_t addr, uint8_t *bytes, uint8_t len)
{
    uint8_t ack = OK;
    uint8_t data = 0;

    // Dummy Write
    ack &= Driver_I2C_Start();
    ack &= Driver_I2C_SendAddr(EEPROM_ADDRESS);
    ack &= Driver_I2C_SendByte(addr);

    // Sequential Read
    ack &= Driver_I2C_Start();
    ack &= Driver_I2C_SendAddr(EEPROM_ADDRESS + 1);

    for (int i = 0; i < len; i++)
    {

        if (i < len - 1)
        {
            Driver_I2C_Ack();
        }
        else
        {
            Driver_I2C_NAck();
        }
        data = Driver_I2C_ReadByte();
        if (data == FAIL)
        {
            ack = FAIL;
        }
        bytes[i] = data;
    }

    Driver_I2C_Stop();

    return ack;
}