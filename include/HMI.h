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

    DateSetting dateSetting;
    TimeSetting timeSetting;
    TemperatureSetting tempSetting;
    Setting settingMode;
    ConstrastSetting contrastSetting;
    BacklightColorsSetting backlightSettings;
    TimeSetting alarmSetting;
    AlarmEnableSetting alarmEnableSetting;

    // Display Mode related functions
    void DisplayMode();
    void DisplaySetting();
    void DisplayDate();
    void DisplayTime();
    void DisplayTemperature();
    void DisplayCurrentState();
    void DisplayContrast();
    void DisplayBacklight();
    void DisplayAlarmSetting();
    void DisplayAlarmEnable();
    void UpdateDisplay();

    // Edit Mode related functions
    void EditMode();
    void EditingDate();
    void EditingTime();
    void ChangeTemp();
    void EditContrast();
    void EditBackLight();
    void EditAlarmTime();
    void EditAlarmEnable();
};
