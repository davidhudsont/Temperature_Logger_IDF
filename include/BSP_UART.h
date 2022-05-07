#pragma once

#include "driver/uart.h"
#include "driver/gpio.h"
#include "stdint.h"

constexpr gpio_num_t UART_TX_PIN_NUM = GPIO_NUM_1;
constexpr gpio_num_t UART_RX_PIN_NUM = GPIO_NUM_3;

constexpr gpio_num_t UART2_TX_PIN_NUM = GPIO_NUM_17;
constexpr gpio_num_t UART2_RX_PIN_NUM = GPIO_NUM_16;

constexpr int UART_RTS = UART_PIN_NO_CHANGE;
constexpr int UART_CTS = UART_PIN_NO_CHANGE;

constexpr int BUFFER_SIZE = 1024;

namespace BSP
{

    class UART
    {
    public:
        UART(int buadrate, int TX_Pin, int RX_Pin, uart_port_t port);
        ~UART();
        UART(const UART &) = delete;
        UART &operator=(const UART &) = delete;

        int BurstRead(uint8_t *data, uint32_t len);
        void WriteBurst(uint8_t *data, uint32_t len);
        void Write(uint8_t data);

        void Flush();

        void GetBufferedDataLength(size_t *len);

        void ToggleRTS();

    private:
        uart_config_t m_config;
        int m_tx_pin;
        int m_rx_pin;
        uart_port_t m_port;
    };

}
