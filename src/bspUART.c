
#include "bspUART.h"
#include "string.h"

/**
 * @brief Configure the UART Device Struct
 * 
 * @param uart_dev - UART device struct to be configured
 * @param buadrate - Buadrate of the device
 * @param TX_Pin   - Transmit Pin
 * @param RX_Pin   - Recieve Pin
 * @param port     - Port number
 */
void UART_CONFIG(UART_STRUCT *uart_dev, int buadrate, int TX_Pin, int RX_Pin, uart_port_t port)
{
    memset(uart_dev, 0, sizeof(UART_STRUCT));

    uart_dev->config.baud_rate = buadrate;
    uart_dev->config.data_bits = UART_DATA_8_BITS;
    uart_dev->config.parity = UART_PARITY_DISABLE;
    uart_dev->config.stop_bits = UART_STOP_BITS_1;
    uart_dev->config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart_dev->config.rx_flow_ctrl_thresh = 0;

    uart_dev->TX_Pin = TX_Pin;
    uart_dev->RX_Pin = RX_Pin;

    uart_dev->port = port;
}

/**
 * @brief Start the UART device
 * 
 * @param uart_dev 
 */
void UART_START_DEVICE(UART_STRUCT *uart_dev)
{
    uart_driver_install(uart_dev->port, BUFFER_SIZE, 0, 0, NULL, 0);
    uart_param_config(uart_dev->port, &uart_dev->config);
    uart_set_pin(uart_dev->port, uart_dev->TX_Pin, uart_dev->RX_Pin, UART_RTS, UART_CTS);
}

/**
 * @brief Read bytes from the UART RX Buffer
 * 
 * @param uart_dev - Device Handle
 * @param data - Data buffer
 * @param len  - Number of bytes to read
 * @return int - Number of bytes found in the buffer
 */
int UART_Read_Bytes(UART_STRUCT *uart_dev, uint8_t *data, uint32_t len)
{
    return uart_read_bytes(uart_dev->port, data, len, 20 / portTICK_RATE_MS);
}

/**
 * @brief Send bytes across the TX pin
 *        The bytes to be written are not buffered.
 * @param uart_dev  - Device Handle
 * @param data - Data buffer
 * @param len - Number of bytes to write
 */
void UART_Write_Bytes(UART_STRUCT *uart_dev, uint8_t *data, uint32_t len)
{
    uart_write_bytes(uart_dev->port, (const char *)data, len);
}
