/**
 * @file TMP102.c
 * @author David Hudson
 * @brief Implementation file for the TMP102 Device
 * @date 2020-01-31
 * 
 */


#include "TMP102.h"
#include "string.h"


/**
 * @brief Start I2C bus and initialize the device structure.
 * 
 * @param tmp102 
 */
void TMP102_Begin(TMP102_STRUCT * tmp102)
{
    memset(tmp102, 0, sizeof(TMP102_STRUCT));

    BSP_I2C_Setup();

}



/**
 * @brief Set the conversion rate of the TMP102
 * 
 * @param tmp102 
 * @param mode 
 */
void TMP102_Set_Conversion_Rate(TMP102_STRUCT * tmp102, CONVERSION_MODES mode)
{
    TMP102_Read_Config(tmp102);
    uint8_t mask = 0x3F;
    tmp102->config[1] &= mask;

    switch (mode)
    {
    case CONVERSION_MODE_0:
        tmp102->config[1] |= CONVERSION_RATE1;
        break;
    case CONVERSION_MODE_1:
        tmp102->config[1] |= CONVERSION_RATE2;
        break;
    case CONVERSION_MODE_2:
        tmp102->config[1] |= CONVERSION_RATE3;
        break;
    case CONVERSION_MODE_3:
        tmp102->config[1] |= CONVERSION_RATE4;
        break;
    default:
        tmp102->config[1] |= CONVERSION_RATE1;
        break;
    }

    TMP102_Write_Config(tmp102);
    
}


/**
 * @brief Put the TMP102 device to sleep
 * 
 * @param tmp102 
 * @param sleep 
 */
void TMP102_Sleep(TMP102_STRUCT * tmp102, bool sleep)
{
    uint8_t config = 0x00;
    TMP102_Read_Register(REG_CONFIG);
    uint8_t mask = 0xFE;
    config &= mask;

    if (sleep)
    {
        config |= CONFIG_SD;
        tmp102->sleep_mode = true;
    }
    else
    {
        config |= 0x00;
        tmp102->sleep_mode = false;
    }
    tmp102->config[0] = config;
    TMP102_Write_Register(REG_CONFIG, config);
}


/**
 * @brief Configure the one shot
 * 
 * @param tmp102 
 */
void TMP102_Set_OneShot(TMP102_STRUCT * tmp102)
{
    uint8_t config = 0x00;
    config = TMP102_Read_Register(REG_CONFIG);
    uint8_t mask = 0x7F;
    config &= mask;

    if (tmp102->sleep_mode)
    {
        config |= CONFIG_ONESHOT;
        tmp102->config[0] = config;
        TMP102_Write_Register(REG_CONFIG, config); 
    }

}


/**
 * @brief Get the current oneshot value.
 * 
 * @param tmp102 
 * @return true if one shot has triggered 
 */
bool TMP102_Get_OneShot(TMP102_STRUCT * tmp102)
{
    uint8_t config = 0x00;
    config = TMP102_Read_Register(REG_CONFIG);

    if ((config & CONFIG_ONESHOT) == CONFIG_ONESHOT)
    {
        return true;
    }
    else
    {
        return false;
    }
}



/**
 * @brief Write to the configuration register with currently stored config value
 * 
 * @param tmp102 
 */
void TMP102_Write_Config(TMP102_STRUCT * tmp102)
{
    BSP_I2C_Write_Burst(TMP102_DEV_ADDR_A, REG_CONFIG, tmp102->config, 2);

}

/**
 * @brief Read the configuration register
 * 
 * @param tmp102 
 */
void TMP102_Read_Config(TMP102_STRUCT * tmp102)
{
    BSP_I2C_Read_Burst(TMP102_DEV_ADDR_A, REG_CONFIG, tmp102->config, 2);

}


/**
 * @brief Reads a sample from the TMP102
 *        and stores the temperature in the TMP102_STRUCT
 * 
 * @param tmp102 
 */
void TMP102_Read_Temperature(TMP102_STRUCT * tmp102)
{
    BSP_I2C_Read_Burst(TMP102_DEV_ADDR_A, REG_TEMPERATURE, tmp102->raw_temp, 2);

    int16_t raw_temp = (((uint16_t) tmp102->raw_temp[0] << 8) | tmp102->raw_temp[1]);

    // The temperature is store in a signed 16bit variable
    // need to convert to 12bit lsb divide by 16(2^4bits)
    raw_temp = raw_temp / CONVERSION16LSB_TO_12LSB;

    // Now that the raw temp is in the right range multiply
    // by temperature scale
    tmp102->temperature = raw_temp * TEMPERATURE_SCALE;

}


/**
 * @brief Return the current sampled temperature in degreees Celsius
 * 
 * @param tmp102 
 * @return float 
 */
float TMP102_Get_Temperature(TMP102_STRUCT * tmp102)
{
    return tmp102->temperature;
}


/**
 * @brief Return the current sampled temperature in degrees Fahrenheit
 * 
 * @param tmp102 
 * @return float 
 */
float TMP102_Get_TemperatureF(TMP102_STRUCT * tmp102)
{
    return tmp102->temperature * (9.0f/5.0f) + 32.0f;
}


/**
 * @brief Read a register from the TMP102
 * 
 * @param address 
 * @return uint8_t 
 */
uint8_t TMP102_Read_Register(uint8_t address)
{
    return BSP_I2C_Read_Byte(TMP102_DEV_ADDR_A, address);
}


/**
 * @brief Write a value to the TMP102 register
 * 
 * @param address 
 * @param data 
 */
void TMP102_Write_Register(uint8_t address, uint8_t data)
{
    BSP_I2C_Write_Byte(TMP102_DEV_ADDR_A, address, data);
}
