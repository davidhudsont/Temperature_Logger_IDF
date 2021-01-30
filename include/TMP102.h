
#ifndef _TMP102_H_
#define _TMP102_H_

#include "bspI2C.h"
#include "TMP102Registers.h"

typedef enum CONVERSION_MODES
{
    CONVERSION_MODE_0 = 0,
    CONVERSION_MODE_1 = 1,
    CONVERSION_MODE_2 = 2,
    CONVERSION_MODE_3 = 3
} CONVERSION_MODES;

typedef struct TMP102_STRUCT
{
    uint8_t raw_temp[2];
    uint8_t config[2];

    uint16_t tlow;
    uint16_t thigh;
    float temperature;

    bool sleep_mode;

} TMP102_STRUCT;

void TMP102_Begin(TMP102_STRUCT *tmp102);

void TMP102_Set_Conversion_Rate(TMP102_STRUCT *tmp102, CONVERSION_MODES mode);
void TMP102_Sleep(TMP102_STRUCT *tmp102, bool sleep);
void TMP102_Set_OneShot(TMP102_STRUCT *tmp102);
bool TMP102_Get_OneShot(TMP102_STRUCT *tmp102);

void TMP102_Write_Config(TMP102_STRUCT *tmp102);
void TMP102_Read_Config(TMP102_STRUCT *tmp102);

void TMP102_Read_Temperature(TMP102_STRUCT *tmp102);

float TMP102_Get_Temperature(TMP102_STRUCT *tmp102);
float TMP102_Get_TemperatureF(TMP102_STRUCT *tmp102);

uint8_t TMP102_Read_Register(uint8_t address);
void TMP102_Write_Register(uint8_t address, uint8_t data);

#endif