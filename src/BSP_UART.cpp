
#include "BSP_UART.h"
#include "string.h"

namespace BSP
{

    /**
     * @brief Start the UART device
     * 
     * @param uart_dev - UART device struct to be configured
     * @param buadrate - Buadrate of the device
     * @param TX_Pin   - Transmit Pin
     * @param RX_Pin   - Recieve Pin
     * @param port     - Port number
     */
    void UART::Setup(int buadrate, int TX_Pin, int RX_Pin, uart_port_t port)
    {
        memset(&m_config, 0, sizeof(uart_config_t));

        m_config.baud_rate = buadrate;
        m_config.data_bits = UART_DATA_8_BITS;
        m_config.parity = UART_PARITY_DISABLE;
        m_config.stop_bits = UART_STOP_BITS_1;
        m_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
        m_config.rx_flow_ctrl_thresh = 0;
        m_config.source_clk = UART_SCLK_APB;

        m_tx_pin = TX_Pin;
        m_rx_pin = RX_Pin;

        m_port = port;

        uart_driver_install(m_port, BUFFER_SIZE, 0, 0, NULL, 0);
        uart_param_config(m_port, &m_config);
        uart_set_pin(m_port, m_tx_pin, m_rx_pin, UART_RTS, UART_CTS);
    }

    /**
     * @brief Read bytes from the UART RX Buffer
     * 
     * @param uart_dev - Device Handle
     * @param data - Data buffer
     * @param len  - Number of bytes to read
     * @return int - Number of bytes found in the buffer
     */
    int UART::BurstRead(uint8_t *data, uint32_t len)
    {
        return uart_read_bytes(m_port, data, len, 20 / portTICK_RATE_MS);
    }

    /**
     * @brief Send bytes across the TX pin
     *        The bytes to be written are not buffered.
     * @param uart_dev 
     * @param data 
     * @param len 
     */
    void UART::WriteBurst(uint8_t *data, uint32_t len)
    {
        uart_write_bytes(m_port, (const char *)data, len);
    }

    /**
     * @brief Send a byte across the TX pin
     * 
     * @param data 
     */
    void UART::Write(uint8_t data)
    {
        char buf[1] = {data};
        uart_write_bytes(m_port, buf, 1);
    }

    void UART::Flush()
    {
        uart_flush(m_port);
    }

    void UART::GetBufferedDataLength(size_t *len)
    {
        uart_get_buffered_data_len(m_port, len);
    }
}
