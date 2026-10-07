#include "Driver_HTTP.h"
#include "Com_Utils.h"
#include "Driver_ESP8266.h"
#include "Driver_USART.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

char http_str[100];
char at_str[200];
char command[100];
char url[100] = "http://192.168.50.171:8080/value";
int port = 8080;

char ssid[] = "wifi_name";
char wifi_pwd[] = "wifi_passwd";

void Driver_HTTP_Init(void)
{
    bool result;

    Driver_ESP8266_Init();
    result = Driver_ESP8266_SendCommand("AT\r\n", "OK", 2000);

    if (!result)
    {
        Driver_USART_SendString((uint8_t *)"ESP8266 ERROR\r\n", 15);
        Driver_USART_SendString(usart1_rx_buffer, usart1_rx_length);
        return;
    }

    result = Driver_ESP8266_SendCommand("AT+CWMODE=1\r\n", "OK", 3000);
    if (!result)
    {
        Driver_USART_SendString((uint8_t *)"CWMODE ERROR\r\n", 14);
        Driver_USART_SendString(usart1_rx_buffer, usart1_rx_length);
        return;
    }

    sprintf(command, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, wifi_pwd);
    result = Driver_ESP8266_SendCommand(command, "OK", 20000);

    if (!result)
    {
        Driver_USART_SendString((uint8_t *)"WIFI ERROR\r\n", 12);
        Driver_USART_SendString(usart1_rx_buffer, usart1_rx_length);
        return;
    }

    Driver_USART_SendString((uint8_t *)"WIFI OK\r\n", 9);
    result = Driver_ESP8266_SendCommand("AT+CWJAP?\r\n", "OK", 3000);

    if (result)
    {
        Driver_USART_SendString((uint8_t *)"\r\nWIFI:\r\n", 9);
        Driver_USART_SendString(usart1_rx_buffer, usart1_rx_length);
    }

    result = Driver_ESP8266_SendCommand("AT+CIPSTA?\r\n", "OK", 3000);
    if (result)
    {
        Driver_USART_SendString((uint8_t *)"\r\nIP:\r\n", 7);
        Driver_USART_SendString(usart1_rx_buffer, usart1_rx_length);
    }

    Driver_USART_SendString((uint8_t *)"\r\nHTTP INIT OK\r\n", 16);
}

bool Driver_HTTP_Post(float value)
{
    char body[50];
    char request[300];

    int body_len = sprintf(body, "{\"value\":%.3f}", value);

    int request_len = sprintf(request,
                              "POST /value HTTP/1.1\r\n"
                              "Host: 192.168.50.171:8080\r\n"
                              "Content-Type: application/json\r\n"
                              "Content-Length: %d\r\n"
                              "Connection: close\r\n"
                              "\r\n"
                              "%s",
                              body_len, body);

    Driver_USART_SendString((uint8_t *)"\r\nHTTP POST\r\n", strlen("\r\nHTTP POST\r\n"));

    /* =====================================================
     * 1. TCP Connect
     * ===================================================== */

    if (!Driver_ESP8266_TCP_Connect("192.168.50.171", 8080))
    {
        Driver_USART_SendString((uint8_t *)"CONNECT FAILED\r\n", strlen("CONNECT FAILED\r\n"));
        Driver_USART_SendString(usart1_rx_buffer, usart1_rx_length);
        Driver_USART_SendString((uint8_t *)"\r\n", 2);

        return false;
    }

    Driver_USART_SendString((uint8_t *)"CONNECT OK\r\n", strlen("CONNECT OK\r\n"));

    /* =====================================================
     * 2. Send HTTP Request
     * ===================================================== */

    if (!Driver_ESP8266_TCP_Send((uint8_t *)request, request_len))
    {
        /*
         * 注意：
         * 即使 TCP_Send() 判斷失敗，
         * HTTP response 可能已經收到。
         *
         * 所以先檢查 HTTP 200。
         */

        if (strstr((char *)usart1_rx_buffer, "HTTP/1.1 200") != NULL)
        {
            Driver_USART_SendString((uint8_t *)"HTTP 200 OK\r\n", strlen("HTTP 200 OK\r\n"));

            Com_Utils_DelayMs(100);
            Driver_ESP8266_TCP_Close();

            return true;
        }

        Driver_USART_SendString((uint8_t *)"SEND FAILED\r\n", strlen("SEND FAILED\r\n"));
        Driver_USART_SendString(usart1_rx_buffer, usart1_rx_length);
        Driver_USART_SendString((uint8_t *)"\r\n", 2);

        Driver_ESP8266_TCP_Close();

        return false;
    }

    Driver_USART_SendString((uint8_t *)"SEND OK\r\n", strlen("SEND OK\r\n"));

    /* =====================================================
     * 3. 等待 HTTP Response
     *
     * 注意：這裡不要 ClearBuffer()
     * ===================================================== */

    uint32_t timeout = 5000;

    while (timeout > 0)
    {
        /* HTTP 200 */
        if (strstr((char *)usart1_rx_buffer, "HTTP/1.1 200") != NULL)
        {
            Driver_USART_SendString((uint8_t *)"HTTP 200 OK\r\n", strlen("HTTP 200 OK\r\n"));

            Com_Utils_DelayMs(100);
            Driver_ESP8266_TCP_Close();

            return true;
        }

        /* 收到其他 HTTP status */
        if (strstr((char *)usart1_rx_buffer, "HTTP/1.1") != NULL)
        {
            Driver_USART_SendString((uint8_t *)"HTTP ERROR\r\n", strlen("HTTP ERROR\r\n"));
            Driver_USART_SendString(usart1_rx_buffer, usart1_rx_length);
            Driver_USART_SendString((uint8_t *)"\r\n", 2);

            Driver_ESP8266_TCP_Close();

            return false;
        }

        Com_Utils_DelayMs(1);
        timeout--;
    }

    /* =====================================================
     * 4. HTTP Response Timeout
     * ===================================================== */

    /*
     * Timeout 前最後再檢查一次。
     * 避免剛好在 timeout 邊界收到 HTTP 200。
     */
    if (strstr((char *)usart1_rx_buffer, "HTTP/1.1 200") != NULL)
    {
        Driver_USART_SendString((uint8_t *)"HTTP 200 OK\r\n", strlen("HTTP 200 OK\r\n"));

        Driver_ESP8266_TCP_Close();

        return true;
    }

    Driver_USART_SendString((uint8_t *)"HTTP RESPONSE TIMEOUT\r\n", strlen("HTTP RESPONSE TIMEOUT\r\n"));
    Driver_USART_SendString(usart1_rx_buffer, usart1_rx_length);
    Driver_USART_SendString((uint8_t *)"\r\n", 2);

    Driver_ESP8266_TCP_Close();

    return false;
}
