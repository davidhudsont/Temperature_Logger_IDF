#pragma once

#include "BSP_I2C.h"
#include "TMP102Registers.h"

enum CONVERSION_MODES
{
    CONVERSION_MODE_0 = CONVERSION_RATE1, // 0.25 Hz
    CONVERSION_MODE_1 = CONVERSION_RATE2, // 1 Hz
    CONVERSION_MODE_2 = CONVERSION_RATE3, // 4 Hz
    CONVERSION_MODE_3 = CONVERSION_RATE4, // 8 Hz
};

enum FAULT_SETTINGS
{
    CFAULTS1 = FAULT_SETTING1, // Consecutive faults 1
    CFAULTS2 = FAULT_SETTING2, // Consecutive faults 2
    CFAULTS4 = FAULT_SETTING3, // Consecutive faults 4
    CFAULTS6 = FAULT_SETTING4, // Consecutive faults 6
};

constexpr float maxTHigh = 150.0f;
constexpr float minTLow = -55.0f;

constexpr int16_t maxPositiveTemp12bit = 0x07FF;
constexpr int16_t maxPositiveTemp13bit = 0x0FFF;

class TMP102
{
private:
    uint8_t config[2];

    float temperature;

    BSP::I2C i2c;

    /**
     * @brief Read a register from the TMP102
     *
     * @param address
     * @return uint8_t
     */
    uint8_t ReadRegister(uint8_t address);

    /**
     * @brief Write a value to the TMP102 register
     * @param address
     * @param data
     */
    void WriteRegister(uint8_t address, uint8_t data);

public:
    /**
     * @brief Construct a new TMP102 object
     *
     */
    TMP102();

    /**
     * @brief Start I2C bus and initialize the device structure.
     */
    void Begin();

    /**
     * @brief Set the conversion rate of the TMP102
     * @param mode
     */
    void SetConversionRate(CONVERSION_MODES mode);

    /**
     * @brief Put the TMP102 device to sleep
     */
    void Sleep();

    /**
     * @brief Wake up the TMP102
     *
     */
    void Wake();

    /**
     * @brief Configure the one shot
     */
    void SetOneShot();

    /**
     * @brief Get the current oneshot value.
     * @return true if one shot has triggered
     */
    bool GetOneShot();

    /**
     * @brief Write to the configuration register with currently stored config value
     */
    void WriteConfig();

    /**
     * @brief Read the configuration register
     */
    void ReadConfig();

    /**
     * @brief Reads a sample from the TMP102
     *        and stores the temperature
     */
    void ReadTemperature();

    /**
     * @brief Return the current sampled temperature in degreees Celsius
     * @return float
     */
    float Temperature();

    /**
     * @brief Return the current sampled temperature in degrees Fahrenheit
     * @return float
     */
    float TemperatureF();
};
