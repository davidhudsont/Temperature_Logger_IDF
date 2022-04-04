#pragma once

#include "driver/i2c.h"

namespace BSP
{

#define SDA_PIN_NUM (21) // I2C SDA PIN Number
#define SCL_PIN_NUM (22) // I2C SCL PIN Number

#define I2C_MASTER_FREQ (100000) // 100 KHz
#define I2C_MASTER_NUM (1)

#define I2C_TIMEOUT (10) // 10ms

    class I2C
    {
    public:
        /**
         * @brief Construct a new I2C object
         *
         */
        I2C();

        void Setup();

        /**
         * @brief Write a byte to a I2C device's register
         *
         * @param dev_address - The device address
         * @param reg_address - The register address
         * @param data - The data to write
         */
        void WriteByte(uint8_t dev_address, uint8_t reg_address, uint8_t data);

        /**
         * @brief Read a byte from a I2C device's register
         *
         * @param dev_address - The device address
         * @param reg_address - The register address
         * @return uint8_t - The data from the register
         */
        uint8_t ReadByte(uint8_t dev_address, uint8_t reg_address);

        /**
         * @brief Read burst from a I2C device
         *
         * @param dev_address - The device address
         * @param reg_address - The register address to start at
         * @param buf - buffer of data to read into
         * @param len - The number of elements
         */
        void WriteBurst(uint8_t dev_address, uint8_t reg_address, uint8_t *buf, uint32_t len);

        /**
         * @brief Write a burst to a I2C device
         *
         * @param dev_address - The device address
         * @param reg_address - The register addres to start at
         * @param buf - Buffer to elements to write
         * @param len - The number of elements
         */
        void ReadBurst(uint8_t dev_address, uint8_t reg_address, uint8_t *buf, uint32_t len);

    private:
        i2c_config_t m_config;
    };

} // namespace BSP
