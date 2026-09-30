
#include "Driver_W5500.h"

char str[100];
int len;
uint8_t ip[4] = {192, 168, 1, 100};
uint8_t mac[6] = {0x00, 0x08, 0xDC, 0x12, 0x34, 0x56};
uint8_t submask[4] = {255, 255, 255, 0};
uint8_t gateway[4] = {192, 168, 1, 1};

void Driver_W5500_Reset()
{
    GPIOB->ODR &= ~GPIO_ODR_ODR10; // low
    Com_Utils_DelayUs(800);
    GPIOB->ODR |= GPIO_ODR_ODR10; // high
    Com_Utils_DelayMs(200);

    Driver_USART_SendString("W5500 RESET\n", 12);
}

void Driver_W5500_Init()
{
    Driver_SPI_Init();
    user_register_function();

    // PWDN - PB1
    GPIOB->CRL |= GPIO_CRL_MODE1;
    GPIOB->CRL &= ~GPIO_CRL_CNF1;
    GPIOB->ODR &= ~GPIO_ODR_ODR1; // low

    // nRESET - PB10
    GPIOB->CRH |= GPIO_CRH_MODE10;
    GPIOB->CRH &= ~GPIO_CRH_CNF10;
    GPIOB->ODR |= GPIO_ODR_ODR10; // high

    // RESET
    Driver_W5500_Reset();

    // set MAC
    setSHAR(mac);
    len = sprintf(str, "MAC: %X-%X-%X-%X-%X-%X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    Driver_USART_SendString((uint8_t *)str, len);
    Com_Utils_DelayMs(500);

    // set IP
    setSIPR(ip);
    setGAR(gateway);
    len = sprintf(str, "IP: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
    Driver_USART_SendString((uint8_t *)str, len);
}