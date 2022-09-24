#pragma once

// User Headers
#include "LCD.h"
#include "DateTime.h"
#include "GlobalSettings.h"

enum HMIState
{
    DISPLAYING,
    EDITING
};

enum HMISettings
{
    SETTING_DATE,
    SETTING_TIME,
    SETTING_TEMP,
    SETTING_CONTRAST,
    SETTING_BACKLIGHT,
    SETTING_ALARM,
    SETTING_ALARM_ENABLE,
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

    HMIState displayState = DISPLAYING;

    Setting settingMode;
    DateSetting dateSetting;
    TimeSetting timeSetting;
    TemperatureSetting tempSetting;
    ConstrastSetting contrastSetting;
    BacklightColorsSetting backlightSettings;
    AlarmSetting alarmSetting;
    AlarmEnableSetting alarmEnableSetting;

    // Display Mode related functions
    void DisplayMode();
    void DisplaySetting();
    void DisplayDate();
    void DisplayTime();
    void DisplayTemperature();
    void DisplayCurrentState();
    void UpdateDisplay();

    // Edit Mode related functions
    void EditMode();
};
