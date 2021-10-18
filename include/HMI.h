#pragma once

#include "LCD.h"
#include "DEADONRTC.h"

enum HMIState
{
    DISPLAYING,
    EDITING
};

struct SETTING
{
    int max_value = 0;
    int min_value = 0;
    int value = 0;

    void adjust(bool increase)
    {
        value = increase ? value + 1 : value - 1;
        if (value > max_value)
            value = min_value;
        else if (value < min_value)
            value = max_value;
    }
};

class HMI
{
private:
    LCD lcd;

    float temperatureF;
    float temperatureC;
    DATE_TIME dateTime;

    enum HMISettings
    {
        SETTING_DATE,
        SETTING_TIME,
        SETTING_TEMP,
        SETTINGS_COUNT
    };

    HMIState displayState = DISPLAYING;
    int entriesToEdit;

    struct DateSetting
    {
        SETTING month;
        SETTING dayOfMonth;
        SETTING year;
    };

    struct TimeSetting
    {
        SETTING hour;
        SETTING minute;
        SETTING second;
    };

    DateSetting dateSetting;
    TimeSetting timeSetting;
    SETTING tempSetting;
    SETTING settingMode;

    // Display Mode related functions
    void displayMode();
    void displayDate();
    void displayTime();
    void displayTemperature();
    void displayCurrentState();
    void updateDisplay();

    // Edit Mode related functions
    void editMode();
    void editingDate();
    void editingTime();
    void changeTemp();

public:
    HMI();
    void process();
    void setDisplayTemperature(float temperatureF, float temperatureC);
    void setDisplayDateTime(DATE_TIME &dateTime);
    HMIState getCurrentState();
};
