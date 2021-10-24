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

struct RGB
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

enum BACKLIGHTCOLORS
{
    RED,
    GREEN,
    BLUE,
    COLOR_COUNT
};

class HMI
{
private:
    LCD lcd;

    float temperatureF;
    float temperatureC;
    bool hour12_not24;
    bool PM_notAM;

    enum HMISettings
    {
        SETTING_DATE,
        SETTING_TIME,
        SETTING_TEMP,
        SETTING_CONTRAST,
        SETTING_BACKLIGHT,
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

    struct BacklightSetting
    {
        SETTING r;
        SETTING g;
        SETTING b;
    };

    RGB backLightValues[COLOR_COUNT];

    DateSetting dateSetting;
    TimeSetting timeSetting;
    SETTING tempSetting;
    SETTING settingMode;
    SETTING contrastSetting;
    SETTING backlightSetting;

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
    void editContrast();
    void editBackLight();

public:
    HMI();
    void process();
    void setDisplayTemperature(float temperatureF, float temperatureC);
    void setDisplayDateTime(DATE_TIME &dateTime);
    HMIState getCurrentState();
};
