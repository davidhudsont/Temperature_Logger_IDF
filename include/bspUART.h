#pragma once

#include "driver/uart.h"
#include "driver/gpio.h"
#include "stdint.h"

#define UART_TX_PIN_NUM (GPIO_NUM_1)
#define UART_RX_PIN_NUM (GPIO_NUM_3)

#define UART2_TX_PIN_NUM (GPIO_NUM_17)
#define UART2_RX_PIN_NUM (GPIO_NUM_16)

#define UART_RTS (UART_PIN_NO_CHANGE)
#define UART_CTS (UART_PIN_NO_CHANGE)

#define BUFFER_SIZE (1024)

typedef struct UART_STRUCT
{
    uart_config_t config;
    uart_port_t port;
    int TX_Pin;
    int RX_Pin;

} UART_STRUCT;

void UART_CONFIG(UART_STRUCT *uart_dev, int buadrate, int TX_Pin, int RX_Pin, uart_port_t port);

void UART_START_DEVICE(UART_STRUCT *uart_dev);

int UART_Read_Bytes(UART_STRUCT *uart_dev, uint8_t *data, uint32_t len);
void UART_Write_Bytes(UART_STRUCT *uart_dev, uint8_t *data, uint32_t len);
