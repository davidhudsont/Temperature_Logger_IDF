#pragma once

#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

namespace BSP
{
#define PIN_NUM_MISO (GPIO_NUM_19)
#define PIN_NUM_MOSI (GPIO_NUM_23)
#define PIN_NUM_SCK (GPIO_NUM_18)
#define PIN_NUM_CS (GPIO_NUM_2)

    class SPI
    {
    private:
        spi_device_handle_t m_spi_handle;

    public:
        /**
         * @brief Construct a new SPI object
         * 
         */
        SPI();

        /**
         * @brief Initializes a SPI device
         * 
         * @param clock_speed - spi_handle data rate
         * @return esp_err_t - Returns an error if the intialization fails
         */
        esp_err_t Initialize(int clock_speed);

        /**
         * @brief reads a 8bit register from a spi_handle device with
         *        address as a command
         * 
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
