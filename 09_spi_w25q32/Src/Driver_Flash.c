#include "Driver_Flash.h"
#include "Driver_USART.h"
#include "stdio.h"
#include "string.h"

void Driver_Flash_Init()
{
    Driver_SPI_Init();
}

void Driver_Flash_WriteEnable(void)
{
    Driver_SPI_Start();
    Driver_SPI_SwapByte(0x06);
    Driver_SPI_Stop();
}

void Driver_Flash_ReadId(uint8_t *mid, uint8_t *did)
{
    Driver_Flash_WaitNotBusy();
    Driver_SPI_Start();
    Driver_SPI_SwapByte(0x90);
    Driver_SPI_SwapByte(0x00);
    Driver_SPI_SwapByte(0x00);
    Driver_SPI_SwapByte(0x00);
    *mid = Driver_SPI_SwapByte(0xFF);
    *did = Driver_SPI_SwapByte(0xFF);
    Driver_SPI_Stop();
}

void Driver_Flash_WaitNotBusy(void)
{
    Driver_SPI_Start();
    Driver_SPI_SwapByte(0x05);
    while (Driver_SPI_SwapByte(0xFF) & 0x01)
        ;
    Driver_SPI_Stop();
}

void Driver_Flash_Read(uint8_t block, uint8_t sector, uint8_t page, uint8_t *data, uint16_t len)
{
    Driver_Flash_WaitNotBusy();
    Driver_SPI_Start();
    Driver_SPI_SwapByte(0x03);
    Driver_SPI_SwapByte(block & 0x3F);
    Driver_SPI_SwapByte((sector << 4) | (page & 0x0F));
    Driver_SPI_SwapByte(0x00);

    for (uint16_t i = 0; i < len; i++)
    {
        data[i] = Driver_SPI_SwapByte(0xFF);
    }

    Driver_SPI_Stop();
}

void Driver_Flash_EraseSector(uint8_t block, uint8_t sector)
{
    Driver_Flash_WaitNotBusy();
    Driver_Flash_WriteEnable();
    Driver_SPI_Start();
    Driver_SPI_SwapByte(0x20);
    Driver_SPI_SwapByte(block & 0x3F);
    Driver_SPI_SwapByte(sector << 4);
    Driver_SPI_SwapByte(0x00);
    Driver_SPI_Stop();
    Driver_Flash_WaitNotBusy();
}

void Driver_Flash_EraseAll()
{
    Driver_Flash_WaitNotBusy();
    Driver_Flash_WriteEnable();
    Driver_SPI_Start();
    Driver_SPI_SwapByte(0x60);
    Driver_SPI_Stop();
    Driver_Flash_WaitNotBusy();
}

void Driver_Flash_WritePage(uint8_t block, uint8_t sector, uint8_t page, uint8_t *data, uint16_t len)
{
    if (len > 256)
    {
        len = 256;
    }

    static uint8_t temp[SECTOR_SIZE];
    Driver_Flash_Read(block, sector, 0, temp, SECTOR_SIZE);

    for (uint16_t i = 0; i < len; i++)
    {
        temp[(uint16_t)page * 256 + i] = data[i];
    }

    Driver_Flash_EraseSector(block, sector);

    for (uint8_t p = 0; p < 16; p++)
    {
        Driver_Flash_WaitNotBusy();
        Driver_Flash_WriteEnable();
        Driver_SPI_Start();
        Driver_SPI_SwapByte(0x02);
        Driver_SPI_SwapByte(block & 0x3F);
        Driver_SPI_SwapByte((sector << 4) | (p & 0x0F));
        Driver_SPI_SwapByte(0x00);
        for (uint16_t j = 0; j < 256; j++)
        {
            Driver_SPI_SwapByte(temp[(uint16_t)p * 256 + j]);
        }
        Driver_SPI_Stop();
        Driver_Flash_WaitNotBusy();
    }
}
