
#include "BSP_I2C.h"
#include <string.h>

namespace BSP
{

    I2C::I2C()
    {
        memset(&m_config, 0, sizeof(i2c_config_t));
    }

    void I2C::Setup()
    {
        memset(&m_config, 0, sizeof(i2c_config_t));

        m_config.mode = I2C_MODE_MASTER;
        m_config.sda_io_num = SDA_PIN_NUM;
        m_config.sda_pullup_en = GPIO_PULLUP_ENABLE;
        m_config.scl_io_num = SCL_PIN_NUM;
        m_config.scl_pullup_en = GPIO_PULLUP_ENABLE;
        m_config.master.clk_speed = I2C_MASTER_FREQ;

        esp_err_t err;
        err = i2c_param_config(I2C_MASTER_NUM, &m_config);
        ESP_ERROR_CHECK(err);
        err = i2c_driver_install(I2C_MASTER_NUM, I2C_MODE_MASTER, 0, 0, 0);
        ESP_ERROR_CHECK(err);
    }

    void I2C::writeByte(uint8_t dev_address, uint8_t reg_address, uint8_t data)
    {
        esp_err_t err;

        // Register Write
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, dev_address << 1 | I2C_MASTER_WRITE, true);
        i2c_master_write_byte(cmd, reg_address, true);
        i2c_master_write_byte(cmd, data, true);
        i2c_master_stop(cmd);
        err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_TIMEOUT / portTICK_RATE_MS);
        if (err != ESP_ERR_TIMEOUT) // Currently don't care about timeout
            ESP_ERROR_CHECK(err);

        i2c_cmd_link_delete(cmd);
    }

    uint8_t I2C::readByte(uint8_t dev_address, uint8_t reg_address)
    {
        esp_err_t err;
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        uint8_t master_write = dev_address << 1 | I2C_MASTER_WRITE;
        uint8_t master_read = dev_address << 1 | I2C_MASTER_READ;

        uint8_t data = 0x00;
        // Register Select
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, master_write, true);
        i2c_master_write_byte(cmd, reg_address, true);
        i2c_master_stop(cmd);
        err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_TIMEOUT / portTICK_RATE_MS);
        if (err != ESP_ERR_TIMEOUT) // Currently don't care about timeout
            ESP_ERROR_CHECK(err);

        i2c_cmd_link_delete(cmd);

        // Register to read
        cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, master_read, true);
        i2c_master_read_byte(cmd, &data, I2C_MASTER_NACK);
        i2c_master_stop(cmd);
        err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_TIMEOUT / portTICK_RATE_MS);
        if (err != ESP_ERR_TIMEOUT) // Currently don't care about timeout
            ESP_ERROR_CHECK(err);

        i2c_cmd_link_delete(cmd);

        return data;
    }

    void I2C::writeBurst(uint8_t dev_address, uint8_t reg_address, uint8_t *buf, uint32_t len)
    {
        esp_err_t err;
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, dev_address << 1 | I2C_MASTER_WRITE, true);
        i2c_master_write_byte(cmd, reg_address, true);
        i2c_master_write(cmd, buf, len, true);
        i2c_master_stop(cmd);
        err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_TIMEOUT / portTICK_RATE_MS);
        if (err != ESP_ERR_TIMEOUT) // Currently don't care about timeout
            ESP_ERROR_CHECK(err);

        i2c_cmd_link_delete(cmd);
    }

    void I2C::readBurst(uint8_t dev_address, uint8_t reg_address, uint8_t *buf, uint32_t len)
    {
        esp_err_t err;
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        uint8_t master_write = dev_address << 1 | I2C_MASTER_WRITE;
        uint8_t master_read = dev_address << 1 | I2C_MASTER_READ;

        // Register Select
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, master_write, true);
        i2c_master_write_byte(cmd, reg_address, I2C_MASTER_LAST_NACK);
        i2c_master_stop(cmd);
        err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_TIMEOUT / portTICK_RATE_MS);
        if (err != ESP_ERR_TIMEOUT) // Currently don't care about timeout
            ESP_ERROR_CHECK(err);

        i2c_cmd_link_delete(cmd);

        // Register Read
        cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, master_read, true);
        i2c_master_read(cmd, buf, len, I2C_MASTER_LAST_NACK);
        i2c_master_stop(cmd);
        err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_TIMEOUT / portTICK_RATE_MS);
        if (err != ESP_ERR_TIMEOUT) // Currently don't care about timeout
            ESP_ERROR_CHECK(err);

        i2c_cmd_link_delete(cmd);
    }

} // namespace BSP