
#include "bspSpi.h"
#include <string.h>

static spi_device_handle_t spi_handle;

static spi_bus_config_t bus_config = {
    .miso_io_num = PIN_NUM_MISO,
    .mosi_io_num = PIN_NUM_MOSI,
    .sclk_io_num = PIN_NUM_SCK,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = 12};

static spi_host_device_t host = VSPI_HOST;

static const int dma_chan = 0;

static spi_device_interface_config_t devcfg = {
    .mode = 3,
    .queue_size = 1,
};

/**
 * @brief Initializes a SPI device on the VSPI Channel
 * 
 * @param clock_speed - spi_handle data rate
 * @return esp_err_t - Returns an error if the intialization fails
 */
esp_err_t BSP_SPI_Init(int clock_speed)
{
    memset(&spi_handle, 0, sizeof(spi_device_handle_t));
    devcfg.clock_speed_hz = clock_speed; // set spi_handle clock speed to passed in parameter
    devcfg.command_bits = 8;             // Set the spi_handle command to a 8bits
    esp_err_t init_fail = false;
    init_fail = spi_bus_initialize(host, &bus_config, dma_chan); // Initialize spi_handle bus per configurations
    ESP_ERROR_CHECK(init_fail);

    esp_err_t device_add_fail = false;
    device_add_fail = spi_bus_add_device(host, &devcfg, &spi_handle); // Add a device to the spi_handle bus
    ESP_ERROR_CHECK(device_add_fail);

    // Not using built in chip select feature of spi driver
    gpio_pad_select_gpio(PIN_NUM_CS);
    gpio_set_direction(PIN_NUM_CS, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_NUM_CS, 1);

    return init_fail | device_add_fail;
}

/**
 * @brief - write 8bits to register with address as a command
 * 
 * @param address - register address to write to
 * @param data - data to be written
 */
void SPI_writeReg(const uint8_t address, const uint8_t data)
{
    spi_transaction_t transaction;
    memset(&transaction, 0, sizeof(transaction));

    transaction.length = 8;
    transaction.flags = SPI_TRANS_USE_TXDATA;
    transaction.tx_data[0] = data;
    transaction.cmd = address;
    esp_err_t ret;
    gpio_set_level(PIN_NUM_CS, 0);
    ret = spi_device_polling_transmit(spi_handle, &transaction);
    gpio_set_level(PIN_NUM_CS, 1);

    assert(ret == ESP_OK);
}

/**
 * @brief reads a 8bit register from a spi_handle device with
 *        address as a command
 * 
 * @param spi_handle - spi_handle device handle
 * @param address - address of the register
 * @return uint8_t - value stored in the register at the address
 */
uint8_t SPI_readReg(const uint8_t address)
{
    spi_transaction_t transaction;
    memset(&transaction, 0, sizeof(transaction));

    transaction.cmd = address;
    transaction.length = 8;
    transaction.flags = SPI_TRANS_USE_RXDATA;

    esp_err_t ret;
    gpio_set_level(PIN_NUM_CS, 0);
    ret = spi_device_polling_transmit(spi_handle, &transaction); //Transmit!
    gpio_set_level(PIN_NUM_CS, 1);
    assert(ret == ESP_OK); //Should have had no issues.

    return transaction.rx_data[0];
}

/**
 * @brief reads bytes into a buffer
 * 
 * @param address - address of the register
 * @param buf - buffer to store read bytes
 * @param size - number of bytes to read
 */
void SPI_Burst_Read(const uint8_t address, uint8_t *buf, uint32_t size)
{
    spi_transaction_t transaction;
    memset(&transaction, 0, sizeof(transaction));

    transaction.cmd = address;
    transaction.length = size * 8;
    transaction.rx_buffer = buf;

    esp_err_t ret;
    gpio_set_level(PIN_NUM_CS, 0);
    ret = spi_device_polling_transmit(spi_handle, &transaction);
    gpio_set_level(PIN_NUM_CS, 1);

    assert(ret == ESP_OK); //Should have had no issues.
}

/**
 * @brief writes bytes at and address 
 * 
 * @param address - address to write to
 * @param buf - buffer to write
 * @param size - number of bytes to write
 */
void SPI_Burst_Write(const uint8_t address, uint8_t *buf, uint32_t size)
{
    spi_transaction_t transaction;
    memset(&transaction, 0, sizeof(transaction));

    transaction.cmd = address;
    transaction.length = size * 8;
    transaction.tx_buffer = buf;

    esp_err_t ret;
    gpio_set_level(PIN_NUM_CS, 0);
    ret = spi_device_polling_transmit(spi_handle, &transaction);
    gpio_set_level(PIN_NUM_CS, 1);
    assert(ret == ESP_OK); //Should have had no issues.
}

#if 0

void SPI_SendByte8(const uint8_t data)
{
    spi_transaction_t transaction;
    memset(&transaction, 0, sizeof(transaction));

    transaction.length = 8;        //Command is 8 bits
    transaction.tx_data[0] = data; //The data is the cmd itself
    transaction.flags = SPI_TRANS_USE_TXDATA;
    printf("Command : %x\n", (unsigned int)transaction.tx_data[0]);
    esp_err_t ret;
    ret = spi_device_polling_transmit(spi_handle, &transaction); //Transmit!
    assert(ret == ESP_OK);                                       //Should have had no issues.
}

uint8_t SPI_GetByte8()
{
    spi_transaction_t transaction;
    memset(&transaction, 0, sizeof(transaction));

    transaction.length = 8;
    transaction.flags = SPI_TRANS_USE_RXDATA;

    esp_err_t ret;
    ret = spi_device_polling_transmit(spi_handle, &transaction); //Transmit!
    assert(ret == ESP_OK);                                       //Should have had no issues.

    return transaction.rx_data[0];
}

void SPI_SendBuffer(uint8_t *buf, uint32_t buffer_length)
{
    spi_transaction_t transaction;
    memset(&transaction, 0, sizeof(transaction));

    // buffer_length is in bytes the transaction length needs to be in bits
    transaction.length = buffer_length * 8;
    transaction.tx_buffer = buf;

    esp_err_t ret;
    ret = spi_device_polling_transmit(spi_handle, &transaction);
    assert(ret == ESP_OK);
}

void SPI_GetBuffer(uint8_t *buf, uint32_t buffer_length)
{
    spi_transaction_t transaction;
    memset(&transaction, 0, sizeof(transaction));

    // buffer_length is in bytes the transaction length needs to be in bits
    transaction.length = buffer_length * 8;

    esp_err_t ret;
    ret = spi_device_polling_transmit(spi_handle, &transaction);
    assert(ret == ESP_OK);

    for (int i = 0; i < buffer_length; i++)
    {
        buf[i] = transaction.rx_data[i];
    }
}

#endif