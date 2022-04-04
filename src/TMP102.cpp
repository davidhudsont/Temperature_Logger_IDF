
#include "TMP102.h"
#include "string.h"

TMP102::TMP102()
    : temperature(0.0f)
{
    memset(config, 0, sizeof(uint8_t) * 2);
}

void TMP102::Setup()
{
    i2c.Setup();
}

void TMP102::SetConversionRate(CONVERSION_MODES mode)
{
    ReadConfig();

    uint8_t mask = 0x3F;
    config[1] &= mask;
    config[1] |= (uint8_t)mode;

    WriteConfig();
}

void TMP102::Sleep()
{
    config[0] = ReadRegister(REG_CONFIG);
    uint8_t mask = 0xFE;
    config[0] &= mask;
    config[0] |= CONFIG_SD;

    WriteRegister(REG_CONFIG, config[0]);
}

void TMP102::Wake()
{
    // Keep all the configured bits except SD
    config[0] = ReadRegister(REG_CONFIG);
    uint8_t mask = 0xFE;
    config[0] &= mask;
    WriteRegister(REG_CONFIG, config[0]);
}

void TMP102::SetOneShot()
{
    config[0] = ReadRegister(REG_CONFIG);
    uint8_t mask = 0x7F;
    config[0] &= mask;
    config[0] |= CONFIG_ONESHOT;
    WriteRegister(REG_CONFIG, config[0]);
}

bool TMP102::GetOneShot()
{
    uint8_t config = 0x00;
    config = ReadRegister(REG_CONFIG);
    // Check to see if os bit is set
    return (config & CONFIG_ONESHOT) == CONFIG_ONESHOT;
}

void TMP102::WriteConfig()
{
    i2c.WriteBurst(TMP102_DEV_ADDR_A, REG_CONFIG, config, 2);
}

void TMP102::ReadConfig()
{
    i2c.ReadBurst(TMP102_DEV_ADDR_A, REG_CONFIG, config, 2);
}

void TMP102::ReadTemperature()
{
    uint8_t data[2];
    i2c.ReadBurst(TMP102_DEV_ADDR_A, REG_TEMPERATURE, data, 2);

    int16_t raw_temperature;
    // If the first bit of second byte is set we are in
    // extended mode.
    if ((data[1] & 0x01) == 0x01)
    {
        // 13 bit conversion
        raw_temperature = (data[0] << 5 | data[1] >> 3);
        if (raw_temperature > maxPositiveTemp13bit)
        {
            // Fill left most bits to take the 2's complements
            raw_temperature |= 0xE000;
        }
    }
    else
    {
        // 12 bit conversion
        raw_temperature = ((data[0]) << 4) | (data[1] >> 4);
        if (raw_temperature > maxPositiveTemp12bit)
        {
            raw_temperature |= 0xF000;
        }
    }

    // Now that the raw temp is in the right range multiply
    // by temperature scale
    temperature = raw_temperature * TEMPERATURE_SCALE;
}

float TMP102::Temperature()
{
    return temperature;
}

float TMP102::TemperatureF()
{
    return temperature * (9.0f / 5.0f) + 32.0f;
}

uint8_t TMP102::ReadRegister(uint8_t address)
{
    return i2c.ReadByte(TMP102_DEV_ADDR_A, address);
}

void TMP102::WriteRegister(uint8_t address, uint8_t data)
{
    i2c.WriteByte(TMP102_DEV_ADDR_A, address, data);
}