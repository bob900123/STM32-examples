#include "Driver_TCP.h"

uint16_t client_port = 5370;
uint16_t server_port = 5000;
uint8_t server_ip[] = {192, 168, 1, 10};

void Driver_TCP_ServerStart(void)
{
    uint8_t status = getSn_SR(SN);

    if (status == SOCK_CLOSED)
    {
        int8_t n = socket(SN, Sn_MR_TCP, server_port, SF_TCP_NODELAY);

        if (n == SN)
        {
            Driver_USART_SendString("Socket OK\n", 10);
        }
        else
        {
            Driver_USART_SendString("Socket Fail\n", 12);
        }
    }
    else if (status == SOCK_INIT)
    {
        int8_t res = listen(SN);
        if (res == SOCK_OK)
        {
            Driver_USART_SendString("Listen OK\n", 10);
        }
        else
        {
            Driver_USART_SendString("Listen Fail\n", 12);
        }
    }
    // else if (status == SOCK_LISTEN)
    // {
    // }
    // else if (status == SOCK_ESTABLISHED)
    // {
    // }
    else if (status == SOCK_CLOSE_WAIT)
    {
        close(SN);
    }
}

void Driver_TCP_ClientStart(void)
{
    static uint8_t link_old = PHY_LINK_OFF;

    uint8_t link = wizphy_getphylink();

    // 網路線斷線
    if (link == PHY_LINK_OFF)
    {
        if (link_old == PHY_LINK_ON)
        {
            Driver_USART_SendString("LAN disconnected\n", 17);
            close(SN);
        }

        link_old = PHY_LINK_OFF;
        return;
    }

    // 網路線重新接上
    if (link_old == PHY_LINK_OFF)
    {
        Driver_USART_SendString("LAN connected\n", 14);
        close(SN);
    }

    link_old = PHY_LINK_ON;
    uint8_t status = getSn_SR(SN);

    if (status == SOCK_CLOSED)
    {
        int8_t n = socket(SN, Sn_MR_TCP, client_port, SF_TCP_NODELAY);

        if (n == SN)
        {
            Driver_USART_SendString("Socket OK\n", 10);
        }
        else
        {
            Driver_USART_SendString("Socket Fail\n", 12);
        }
    }
    else if (status == SOCK_INIT)
    {
        int8_t res = connect(SN, server_ip, server_port);
        if (res == SOCK_OK)
        {
            Driver_USART_SendString("Connect OK\n", 11);
        }
        else
        {
            Driver_USART_SendString("Connect Fail\n", 13);
        }
    }
    // else if (status == SOCK_LISTEN)
    // {
    // }
    // else if (status == SOCK_ESTABLISHED)
    // {
    // }
    else if (status == SOCK_CLOSE_WAIT)
    {
        Driver_USART_SendString("Socket Closed\n", 14);
        disconnect(SN);
        close(SN);
    }
}

void Driver_TCP_RecvData(uint8_t buff[], uint16_t *len)
{
    uint8_t status = getSn_SR(SN);

    if (status == SOCK_ESTABLISHED)
    {
        if (getSn_IR(SN) & Sn_IR_RECV)
        {
            setSn_IR(SN, Sn_IR_RECV);
            *len = getSn_RX_RSR(SN);
            recv(SN, buff, *len);
            buff[*len] = '\0';
        }
    }
}

void Driver_TCP_SendData(uint8_t data[], uint16_t len)
{
    uint8_t status = getSn_SR(SN);

    if (status == SOCK_ESTABLISHED)
    {
        send(SN, data, len);
    }
}