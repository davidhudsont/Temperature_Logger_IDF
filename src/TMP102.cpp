
#include "TMP102.h"
#include "string.h"

void TMP102::Begin()
{
    memset(raw_temp, 0, sizeof(uint8_t) * 2);
    memset(config, 0, sizeof(uint8_t) * 2);
    tlow = 0;
    thigh = 0;
    temperature = 0.0f;
    sleep_mode = false;
    i2c.Setup();
}

void TMP102::Set_Conversion_Rate(CONVERSION_MODES mode)
{
    Read_Config();

    uint8_t mask = 0x3F;
    config[1] &= mask;

    switch (mode)
    {
    case CONVERSION_MODE_0:
        config[1] |= CONVERSION_RATE1;
        break;
    case CONVERSION_MODE_1:
        config[1] |= CONVERSION_RATE2;
        break;
    case CONVERSION_MODE_2:
        config[1] |= CONVERSION_RATE3;
        break;
    case CONVERSION_MODE_3:
        config[1] |= CONVERSION_RATE4;
        break;
    default:
        config[1] |= CONVERSION_RATE1;
        break;
    }

    Write_Config();
}

void TMP102::Sleep(bool sleep)
{
    uint8_t sleep_config = 0x00;
    Read_Register(REG_CONFIG);
    uint8_t mask = 0xFE;
    sleep_config &= mask;

    if (sleep)
    {
        sleep_config |= CONFIG_SD;
        sleep_mode = true;
    }
    else
    {
        sleep_config |= 0x00;
        sleep_mode = false;
    }
    config[0] = sleep_config;
    Write_Register(REG_CONFIG, sleep_config);
}

void TMP102::Set_OneShot()
{
    uint8_t oneshot_config = 0x00;
    oneshot_config = Read_Register(REG_CONFIG);
    uint8_t mask = 0x7F;
    oneshot_config &= mask;

    if (sleep_mode)
    {
        oneshot_config |= CONFIG_ONESHOT;
        config[0] = oneshot_config;
        Write_Register(REG_CONFIG, oneshot_config);
    }
}

bool TMP102::Get_OneShot()
{
    uint8_t config = 0x00;
    config = Read_Register(REG_CONFIG);

    if ((config & CONFIG_ONESHOT) == CONFIG_ONESHOT)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void TMP102::Write_Config()
{
    i2c.writeBurst(TMP102_DEV_ADDR_A, REG_CONFIG, config, 2);
}

void TMP102::Read_Config()
{
    i2c.readBurst(TMP102_DEV_ADDR_A, REG_CONFIG, config, 2);
}

void TMP102::Read_Temperature()
{
    i2c.readBurst(TMP102_DEV_ADDR_A, REG_TEMPERATURE, raw_temp, 2);

    int16_t raw_temperature = (((uint16_t)raw_temp[0] << 8) | raw_temp[1]);

    // The temperature is store in a signed 16bit variable
    // need to convert to 12bit lsb divide by 16(2^4bits)
    raw_temperature = raw_temperature / CONVERSION16LSB_TO_12LSB;

    // Now that the raw temp is in the right range multiply
    // by temperature scale
    temperature = raw_temperature * TEMPERATURE_SCALE;
}

float TMP102::Get_Temperature()
{
    return temperature;
}

float TMP102::Get_TemperatureF()
{
    return temperature * (9.0f / 5.0f) + 32.0f;
}

uint8_t TMP102::Read_Register(uint8_t address)
{
    return i2c.readByte(TMP102_DEV_ADDR_A, address);
}

void TMP102::Write_Register(uint8_t address, uint8_t data)
{
    i2c.writeByte(TMP102_DEV_ADDR_A, address, data);
}