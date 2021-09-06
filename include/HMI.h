#pragma once

#include "LCD.h"
#include "Button.h"
#include "DeviceCommands.h"
#include "DEADONRTC.h"

class HMI
{
private:
    LCD lcd;

    bool displayTempF_notC;
    float temperatureF;
    float temperatureC;
    DATE_TIME dateTime;

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

    HMIState displayState = DISPLAYING;
    HMISettings settingState = SETTING_DATE;
    int entriesToEdit;

    void displayCurrentState();
    void editMonth(bool increase);
    void editDayOfMonth(bool increase);
    void editYear(bool increase);
    void editingDate();
    void editingTime();
    void changeTemp();

    void editing();
    void display();
    void update();

public:
    HMI();
    void process();
    void setDisplayTemperature(float temperatureF, float temperatureC);
    void setDisplayDateTime(DATE_TIME &dateTime);
};
