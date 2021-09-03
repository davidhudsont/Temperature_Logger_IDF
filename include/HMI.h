#pragma once

#include "LCD.h"
#include "Button.h"
#include "DeviceCommands.h"
#include "DEADONRTC.h"

enum HMIState
{
    DISPLAYING,
    EDITING
};

enum HMISettings
{
    SETTING_DATE,
    SETTING_TIME,
    SETTING_TEMP
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

    HMIState displayState = DISPLAYING;
    HMISettings settingState = SETTING_DATE;

    void editing();
    void display();

public:
    HMI();
    void process();
    void updateDisplayTemperature(float temperatureF, float temperatureC);
    void updateDisplayDateTime(DATE_TIME &dateTime);
};