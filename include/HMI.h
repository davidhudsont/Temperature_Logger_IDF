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
    LOW_BRIGHT,
    MED_BRIGHT,
    FULL_BRIGHT,
    COLOR_COUNT,
};

enum HMISettings
{
    SETTING_DATE,
    SETTING_TIME,
    SETTING_TEMP,
    SETTING_CONTRAST,
    SETTING_BACKLIGHT,
    SETTING_ALARM,
    SETTINGS_COUNT,
};

class HMI
{
public:
    HMI();
    ~HMI() = default;
    HMI(const HMI &) = delete;
    HMI &operator=(const HMI &) = delete;

    void Process();
    void SetDisplayTemperature(float temperatureF, float temperatureC);
    void SetDisplayDateTime(DATE_TIME &dateTime);
    HMIState GetCurrentState();
    void Reset();

private:
    LCD lcd;

    float temperatureF;
    float temperatureC;
    bool hour12_not24;
    bool PM_notAM;

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
        SETTING PM_notAM;
    };

    struct BacklightSetting
    {
        SETTING r;
        SETTING g;
        SETTING b;
    };

    struct AlarmSetting
    {
        SETTING hour;
        SETTING minute;
        SETTING PM_notAM;
    };

    RGB backLightValues[COLOR_COUNT];

    DateSetting dateSetting;
    TimeSetting timeSetting;
    SETTING tempSetting;
    SETTING settingMode;
    SETTING contrastSetting;
    SETTING backlightSetting;
    AlarmSetting alarmSetting;

    // Display Mode related functions
    void DisplayMode();
    void DisplayDate();
    void DisplayTime();
    void DisplayTemperature();
    void DisplayCurrentState();
    void DisplayContrast();
    void DisplayBacklight();
    void DisplayAlarmSetting();
    void UpdateDisplay();

    // Edit Mode related functions
    void EditMode();
    void EditingDate();
    void EditingTime();
    void ChangeTemp();
    void EditContrast();
    void EditBackLight();
    void EditAlarmTime();
};
