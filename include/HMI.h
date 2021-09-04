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

class HMI
{
private:
    LCD lcd;

    bool displayTempF_notC;
    float temperatureF;
    float temperatureC;
    DATE_TIME dateTime;

    HMIState displayState = DISPLAYING;
    HMISettings settingState = SETTING_DATE;

    void editing();
    void display();
    void update();

public:
    HMI();
    void process();
    void setDisplayTemperature(float temperatureF, float temperatureC);
    void setDisplayDateTime(DATE_TIME &dateTime);
};
