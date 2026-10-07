#ifndef DRIVER_ESP8266_H
#define DRIVER_ESP8266_H

#include "stm32f10x.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define ESP8266_DMA_RX_SIZE    100
#define ESP8266_RX_BUFFER_SIZE 500

extern uint8_t usart1_rx_buffer[ESP8266_RX_BUFFER_SIZE];
extern volatile uint16_t usart1_rx_length;
extern volatile bool usart1_is_receive_finished;
extern volatile bool usart1_is_transmit_finished;

void Driver_ESP8266_Init(void);
void Driver_ESP8266_SendString(const uint8_t *str, uint16_t len);
void Driver_ESP8266_ClearBuffer(void);
bool Driver_ESP8266_WaitFor(const char *expected, uint32_t timeout_ms);
bool Driver_ESP8266_SendCommand(const char *command, const char *expected, uint32_t timeout_ms);
void Driver_ESP8266_TCP_Close(void);
bool Driver_ESP8266_TCP_Connect(const char *ip, uint16_t port);
bool Driver_ESP8266_TCP_Send(const uint8_t *data, uint16_t len);

#endif