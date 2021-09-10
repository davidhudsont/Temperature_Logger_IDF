
#include "BSP_SPI.h"
#include <string.h>

namespace BSP
{
    SPI::SPI()
    {
        memset(&m_spi_handle, 0, sizeof(spi_device_handle_t));
    }

    esp_err_t SPI::Initialize(int clock_speed)
    {
        memset(&m_spi_handle, 0, sizeof(spi_device_handle_t));
        spi_bus_config_t spiBusConfig;
        spiBusConfig.miso_io_num = PIN_NUM_MISO;
        spiBusConfig.mosi_io_num = PIN_NUM_MOSI;
        spiBusConfig.sclk_io_num = PIN_NUM_SCK;
        spiBusConfig.quadwp_io_num = -1;
        spiBusConfig.quadhd_io_num = -1;
        spiBusConfig.max_transfer_sz = 12;
        spiBusConfig.flags = 0;
        spiBusConfig.intr_flags = 0;

        spi_host_device_t host = VSPI_HOST;
        int dma_chan = 0;

        spi_device_interface_config_t devcfg;
        devcfg.mode = 3;
        devcfg.queue_size = 1;
        // set spi_handle clock speed to passed in parameter
        devcfg.clock_speed_hz = clock_speed;
        // Set the spi_handle command to a 8bits
        devcfg.command_bits = 8;

        esp_err_t init_fail = false;
        // Initialize spi_handle bus per configurations
        init_fail = spi_bus_initialize(host, &spiBusConfig, dma_chan);
        ESP_ERROR_CHECK(init_fail);

        esp_err_t device_add_fail = false;
        // Add a device to the spi_handle bus
        device_add_fail = spi_bus_add_device(host, &devcfg, &m_spi_handle);
        ESP_ERROR_CHECK(device_add_fail);

        // Not using built in chip select feature of spi driver
        gpio_pad_select_gpio(PIN_NUM_CS);
        gpio_set_direction(PIN_NUM_CS, GPIO_MODE_OUTPUT);
        gpio_set_level(PIN_NUM_CS, 1);

        return init_fail | device_add_fail;
    }

    uint8_t SPI::readReg(const uint8_t address)
    {
        spi_transaction_t transaction;
        memset(&transaction, 0, sizeof(transaction));

        transaction.cmd = address;
        transaction.length = 8;
        transaction.flags = SPI_TRANS_USE_RXDATA;

        esp_err_t err;
        gpio_set_level(PIN_NUM_CS, 0);
        err = spi_device_polling_transmit(m_spi_handle, &transaction);
        gpio_set_level(PIN_NUM_CS, 1);
        assert(err == ESP_OK);

        return transaction.rx_data[0];
    }

    void SPI::writeReg(const uint8_t address, const uint8_t data)
    {
        spi_transaction_t transaction;
        memset(&transaction, 0, sizeof(transaction));

        transaction.length = 8;
        transaction.flags = SPI_TRANS_USE_TXDATA;
        transaction.tx_data[0] = data;
        transaction.cmd = address;

        esp_err_t err;
        gpio_set_level(PIN_NUM_CS, 0);
        err = spi_device_polling_transmit(m_spi_handle, &transaction);
        gpio_set_level(PIN_NUM_CS, 1);
        assert(err == ESP_OK);
    }

    void SPI::burstRead(const uint8_t address, uint8_t *buf, uint32_t size)
    {
        spi_transaction_t transaction;
        memset(&transaction, 0, sizeof(transaction));

        transaction.cmd = address;
        transaction.length = size * 8;
        transaction.rx_buffer = buf;

        esp_err_t err;
        gpio_set_level(PIN_NUM_CS, 0);
        err = spi_device_polling_transmit(m_spi_handle, &transaction);
        gpio_set_level(PIN_NUM_CS, 1);

        assert(err == ESP_OK);
    }

    void SPI::burstWrite(const uint8_t address, uint8_t *buf, uint32_t size)
    {
        spi_transaction_t transaction;
        memset(&transaction, 0, sizeof(transaction));

        transaction.cmd = address;
        transaction.length = size * 8;
        transaction.tx_buffer = buf;

        esp_err_t err;
        gpio_set_level(PIN_NUM_CS, 0);
        err = spi_device_polling_transmit(m_spi_handle, &transaction);
        gpio_set_level(PIN_NUM_CS, 1);
        assert(err == ESP_OK);
    }

} // namespace BSP
