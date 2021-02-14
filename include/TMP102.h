
#ifndef _TMP102_H_
#define _TMP102_H_

#include "BSP_I2C.h"
#include "TMP102Registers.h"
#include <string>

typedef enum CONVERSION_MODES
{
    CONVERSION_MODE_0 = 0,
    CONVERSION_MODE_1 = 1,
    CONVERSION_MODE_2 = 2,
    CONVERSION_MODE_3 = 3
} CONVERSION_MODES;

class TMP102
{
private:
    uint8_t raw_temp[2];
    uint8_t config[2];

    uint16_t tlow;
    uint16_t thigh;
    float temperature;

    bool sleep_mode;

    BSP::I2C i2c;

    /**
     * @brief Read a register from the TMP102
     * 
     * @param address 
     * @return uint8_t 
     */
    uint8_t Read_Register(uint8_t address);

    /**
     * @brief Write a value to the TMP102 register
     * @param address 
     * @param data 
     */
    void Write_Register(uint8_t address, uint8_t data);

public:
    /**
     * @brief Start I2C bus and initialize the device structure.
     */
    void Begin();

    /**
     * @brief Set the conversion rate of the TMP102
     * @param mode 
     */
    void Set_Conversion_Rate(CONVERSION_MODES mode);

    /**
     * @brief Put the TMP102 device to sleep
     * @param bool sleep 
     */
    void Sleep(bool sleep);

    /**
     * @brief Configure the one shot
     */
    void Set_OneShot();

    /**
     * @brief Get the current oneshot value.
     * @return true if one shot has triggered 
     */
    bool Get_OneShot();

    /**
     * @brief Write to the configuration register with currently stored config value
     */
    void Write_Config();

    /**
     * @brief Read the configuration register
     */
    void Read_Config();

    /**
     * @brief Reads a sample from the TMP102
     *        and stores the temperature
     */
    void Read_Temperature();

    /**
     * @brief Return the current sampled temperature in degreees Celsius
     * @return float 
     */
    float Get_Temperature();

    /**
     * @brief Return the current sampled temperature in degrees Fahrenheit
     * @return float 
     */
    float Get_TemperatureF();

    /**
     * @brief Get a string of the temperature in Fahrenheit
     * 
     * @return std::string 
     */
    std::string Get_TemperatureF_ToString();

    /**
     * @brief Get a string of the temperature in Celsius
     * 
     * @return std::string 
     */
    std::string Get_TemperatureC_ToString();
};

#endif