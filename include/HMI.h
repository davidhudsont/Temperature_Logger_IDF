#pragma once

#include "LCD.h"
#include "Button.h"
#include "DeviceCommands.h"

enum LCDState
{
    DISPLAYING,
    EDITING
};

enum LCDSettings
{
    SETTING_DATE,
    SETTING_TIME,
    SETTING_TEMP
};

struct DATE_TIME
{
    uint8_t year;
    uint8_t month;
    uint8_t dayofMonth;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    bool hour12_not24;
    bool PM_notAM;
};

const gpio_num_t editPin = GPIO_NUM_13;
const gpio_num_t settingModePin = GPIO_NUM_12;
const gpio_num_t downPin = GPIO_NUM_14;
const gpio_num_t upPin = GPIO_NUM_27;

class HMI
{
private:
    LCD lcd;
    Button editButton;
    Button settingModeButton;
    Button downButton;
    Button upButton;

    bool displayTempF_notC;
    float temperatureF;
    float temperatureC;
    DATE_TIME dateTime;

    LCDState displayState = DISPLAYING;
    LCDSettings settingState = SETTING_DATE;

    void editing();
    void display();

public:
    HMI();
    void process();
    void updateDisplayTemperature(float temperatureF, float temperatureC);
    void updateDisplayDateTime(DATE_TIME &dateTime);
};