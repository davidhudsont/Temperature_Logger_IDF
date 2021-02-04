
#ifndef __BSP_SPI_H__
#define __BSP_SPI_H__

#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "driver/spi_slave.h"
#include "driver/gpio.h"
#include <string.h>

namespace BSP
{
#define PIN_NUM_MISO (GPIO_NUM_19)
#define PIN_NUM_MOSI (GPIO_NUM_23)
#define PIN_NUM_SCK (GPIO_NUM_18)
#define PIN_NUM_CS (GPIO_NUM_2)

    class SPI
    {
    private:
        spi_bus_config_t m_bus_config;
        spi_host_device_t m_host;
        int m_dma_chan;
        spi_device_interface_config_t m_devcfg;
        spi_device_handle_t m_spi_handle;

        struct SPI_TRANSACTION
        {
            spi_transaction_t transaction;
            SPI_TRANSACTION()
            {
                memset(&transaction, 0, sizeof(transaction));
            }
            void transmit(spi_device_handle_t &handle)
            {
                esp_err_t err;
                gpio_set_level(PIN_NUM_CS, 0);
                err = spi_device_polling_transmit(handle, &transaction);
                gpio_set_level(PIN_NUM_CS, 1);
                assert(err == ESP_OK);
            }
        };

    public:
        /**
         * @brief Initializes a SPI device on the VSPI Channel
         * 
         * @param clock_speed - spi_handle data rate
         * @return esp_err_t - Returns an error if the intialization fails
         */
        esp_err_t Init(int clock_speed);

        /**
         * @brief reads a 8bit register from a spi_handle device with
         *        address as a command
         * 
         * @param spi_handle - spi_handle device handle
         * @param address - address of the register
         * @return uint8_t - value stored in the register at the address
         */
        uint8_t readReg(const uint8_t address);

        /**
         * @brief - write 8bits to register with address as a command
         * 
         * @param address - register address to write to
         * @param data - data to be written
         */
        void writeReg(const uint8_t address, const uint8_t data);

        /**
         * @brief reads bytes into a buffer
         * 
         * @param address - address of the register
         * @param buf - buffer to store read bytes
         * @param size - number of bytes to read
         */
        void burstRead(const uint8_t address, uint8_t *buf, uint32_t size);

        /**
         * @brief writes bytes at and address 
         * 
         * @param address - address to write to
         * @param buf - buffer to write
         * @param size - number of bytes to write
         */
        void burstWrite(const uint8_t address, uint8_t *buf, uint32_t size);
    };
} // namespace BSP

#endif