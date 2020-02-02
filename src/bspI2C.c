

#include "bspI2C.h"
#include "sdkconfig.h"
#include <string.h>


/**
 * @brief Setup the I2C bus as master
 * 
 */
void BSP_I2C_Setup()
{
    i2c_config_t config;
    memset(&config, 0, sizeof(i2c_config_t));

    config.mode = I2C_MODE_MASTER;
    config.sda_io_num = SDA_PIN_NUM;
    config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    config.scl_io_num = SCL_PIN_NUM;
    config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    config.master.clk_speed = I2C_MASTER_FREQ;

    esp_err_t err;
    err = i2c_param_config(I2C_MASTER_NUM, &config);
    ESP_ERROR_CHECK(err);
    err = i2c_driver_install(I2C_MASTER_NUM, I2C_MODE_MASTER, 0, 0, 0);
    ESP_ERROR_CHECK(err);

}


/**
 * @brief Write a byte to a I2C device's register
 * 
 * @param dev_address - The device address
 * @param reg_address - The register address
 * @param data - The data to write
 */
void BSP_I2C_Write_Byte(uint8_t dev_address, uint8_t reg_address, uint8_t data)
{
    esp_err_t err;

    // Register Write
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, dev_address << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_address, true);
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);
    err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_TIMEOUT/portTICK_RATE_MS);
    if (err != ESP_ERR_TIMEOUT) // Currently don't care about timeout
        ESP_ERROR_CHECK(err);
        
    i2c_cmd_link_delete(cmd);

}


/**
 * @brief Read a byte from a I2C device's register
 * 
 * @param dev_address - The device address
 * @param reg_address - The register address
 * @return uint8_t - The data from the register
 */
uint8_t BSP_I2C_Read_Byte(uint8_t dev_address, uint8_t reg_address)
{
    esp_err_t err;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    uint8_t master_write = dev_address << 1 | I2C_MASTER_WRITE;
    uint8_t master_read  = dev_address << 1 | I2C_MASTER_READ;
    
    uint8_t data = 0x00;
    // Register Select
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, master_write, true);
    i2c_master_write_byte(cmd, reg_address, true);
    i2c_master_stop(cmd);
    err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_TIMEOUT/portTICK_RATE_MS);
    if (err != ESP_ERR_TIMEOUT) // Currently don't care about timeout
        ESP_ERROR_CHECK(err);

    i2c_cmd_link_delete(cmd);

    // Register to read
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, master_read, true);
    i2c_master_read_byte(cmd, &data, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_TIMEOUT/portTICK_RATE_MS);
    if (err != ESP_ERR_TIMEOUT) // Currently don't care about timeout
        ESP_ERROR_CHECK(err);

    i2c_cmd_link_delete(cmd);

    return data;
}



/**
 * @brief Read burst from a I2C device
 * 
 * @param dev_address - The device address
 * @param reg_address - The register address to start at
 * @param buf - buffer of data to read into
 * @param len - The number of elements
 */
void BSP_I2C_Read_Burst(uint8_t dev_address, uint8_t reg_address, uint8_t * buf, uint32_t len)
{
    esp_err_t err;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    uint8_t master_write = dev_address << 1 | I2C_MASTER_WRITE;
    uint8_t master_read  = dev_address << 1 | I2C_MASTER_READ;

    // Register Select
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, master_write, true);
    i2c_master_write_byte(cmd, reg_address, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_TIMEOUT/portTICK_RATE_MS);
    if (err != ESP_ERR_TIMEOUT) // Currently don't care about timeout
        ESP_ERROR_CHECK(err);

    i2c_cmd_link_delete(cmd);

    // Register Read
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, master_read, true);
    i2c_master_read(cmd, buf, len, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_TIMEOUT/portTICK_RATE_MS);
    if (err != ESP_ERR_TIMEOUT) // Currently don't care about timeout
        ESP_ERROR_CHECK(err);

    i2c_cmd_link_delete(cmd);

}


/**
 * @brief Write a burst to a I2C device
 * 
 * @param dev_address - The device address
 * @param reg_address - The register addres to start at
 * @param buf - Buffer to elements to write
 * @param len - The number of elements
 */
void BSP_I2C_Write_Burst(uint8_t dev_address, uint8_t reg_address, uint8_t * buf, uint32_t len)
{
    esp_err_t err;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, dev_address << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_address, true);
    i2c_master_write(cmd, buf, len, true);
    i2c_master_stop(cmd);
    err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, I2C_TIMEOUT/portTICK_RATE_MS);
    if (err != ESP_ERR_TIMEOUT) // Currently don't care about timeout
        ESP_ERROR_CHECK(err);

    i2c_cmd_link_delete(cmd);

}