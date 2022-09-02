#include "GlobalSettings.h"

#include "DeviceCommands.h"
#include "DateTime.h"

DateSetting::DateSetting()
{
    Setting year = Setting("year", 0, 99, 0);
    Setting month = Setting("month", 1, 12, 1);
    Setting dayOfMonth = Setting("dayOfMonth", 1, 31, 1);

    addSetting(year);
    addSetting(month);
    addSetting(dayOfMonth);
}

bool DateSetting::getInput(const Input &input)
{
    static int entriesToEdit = 3;
    if (input == Input::UP)
    {
        if (entriesToEdit == 3)
        {
            settingsList[0].increment();
        }
        else if (entriesToEdit == 2)
        {
            settingsList[1].increment();
        }
        else if (entriesToEdit == 1)
        {
            settingsList[2].increment();
        }
    }
    else if (input == Input::DOWN)
    {
        if (entriesToEdit == 3)
        {
            settingsList[0].decrement();
        }
        else if (entriesToEdit == 2)
        {
            settingsList[1].decrement();
        }
        else if (entriesToEdit == 1)
        {
            settingsList[2].decrement();
        }
    }
    else if (input == Input::ENTER)
    {
        entriesToEdit--;
        if (entriesToEdit == 1)
        {
            uint8_t year = settingsList[0].get();
            uint8_t month = settingsList[1].get();
            uint8_t max_value = calculateMaxDayOfMonth(month, year);
            settingsList[2].set_max(max_value);
        }
        if (entriesToEdit == 0)
        {
            uint8_t year = settingsList[0].get();
            uint8_t month = settingsList[1].get();
            uint8_t dayOfMonth = settingsList[2].get();
            SetDate(dayOfMonth, month, year);
            entriesToEdit = 3;
            return true;
        }
    }
    return false;
}

std::string DateSetting::displayString()
{
    constexpr size_t dateStringSize = 15;
    static char dateString[dateStringSize] = "";

    uint8_t year = settingsList[0].get();
    uint8_t month = settingsList[1].get();
    uint8_t dayOfMonth = settingsList[2].get();

    snprintf(dateString, dateStringSize, "%02d/%02d/%d",
             month,
             dayOfMonth,
             year + 2000);

    return std::string(dateString, 10);
}

TimeSetting::TimeSetting()
{
    Setting hour = Setting("hour", 1, 12, 1);
    Setting minute = Setting("minute", 1, 59, 1);
    Setting amPm = Setting("amPm", 0, 1, 0);

    addSetting(hour);
    addSetting(minute);
    addSetting(amPm);
}

bool TimeSetting::getInput(const Input &input)
{
    static int entriesToEdit = 3;
    if (input == Input::UP)
    {
        if (entriesToEdit == 3)
        {
            settingsList[0].increment();
        }
        else if (entriesToEdit == 2)
        {
            settingsList[1].increment();
        }
        else if (entriesToEdit == 1)
        {
            settingsList[2].increment();
        }
    }
    else if (input == Input::DOWN)
    {
        if (entriesToEdit == 3)
        {
            settingsList[0].decrement();
        }
        else if (entriesToEdit == 2)
        {
            settingsList[1].decrement();
        }
        else if (entriesToEdit == 1)
        {
            settingsList[2].decrement();
        }
    }
    else if (input == Input::ENTER)
    {
        entriesToEdit--;
        if (entriesToEdit == 0)
        {
            uint8_t hour = settingsList[0].get();
            uint8_t minute = settingsList[1].get();
            uint8_t amPm = settingsList[2].get();
            SetTime12(hour, minute, amPm);
            entriesToEdit = 3;
            return true;
        }
    }
    return false;
}

std::string TimeSetting::displayString()
{
    constexpr size_t timeStringSize = 13;
    static char timeString[timeStringSize] = "";

    uint8_t hour = settingsList[0].get();
    uint8_t minute = settingsList[1].get();
    uint8_t amPm = settingsList[2].get();

    snprintf(timeString, timeStringSize, "%02d:%02d %s",
             hour,
             minute,
             amPm ? "PM" : "AM");
    return std::string(timeString, 8);
}

AlarmSetting::AlarmSetting()
{
    Setting hour = Setting("hour", 1, 12, 1);
    Setting minute = Setting("minute", 1, 59, 1);
    Setting amPm = Setting("amPm", 0, 1, 0);

    addSetting(hour);
    addSetting(minute);
    addSetting(amPm);
}

std::string AlarmSetting::displayString()
{
    constexpr size_t timeStringSize = 13;
    static char timeString[timeStringSize] = "";

    uint8_t hour = settingsList[0].get();
    uint8_t minute = settingsList[1].get();
    uint8_t amPm = settingsList[2].get();

    snprintf(timeString, timeStringSize, "%02d:%02d %s",
             hour,
             minute,
             amPm ? "PM" : "AM");
    return std::string(timeString, 8);
}

bool AlarmSetting::getInput(const Input &input)
{
    static int entriesToEdit = 3;
    if (input == Input::UP)
    {
        if (entriesToEdit == 3)
        {
            settingsList[0].increment();
        }
        else if (entriesToEdit == 2)
        {
            settingsList[1].increment();
        }
        else if (entriesToEdit == 1)
        {
            settingsList[2].increment();
        }
    }
    else if (input == Input::DOWN)
    {
        if (entriesToEdit == 3)
        {
            settingsList[0].decrement();
        }
        else if (entriesToEdit == 2)
        {
            settingsList[1].decrement();
        }
        else if (entriesToEdit == 1)
        {
            settingsList[2].decrement();
        }
    }
    else if (input == Input::ENTER)
    {
        entriesToEdit--;
        if (entriesToEdit == 0)
        {
            uint8_t hour = settingsList[0].get();
            uint8_t minute = settingsList[1].get();
            uint8_t amPm = settingsList[2].get();
            SetAlarmTime12(hour, minute, amPm);
            entriesToEdit = 3;
            return true;
        }
    }
    return false;
}

AlarmEnableSetting::AlarmEnableSetting()
{
    Setting enable = Setting("enable", 0, 1, 0);
    addSetting(enable);
}

std::string AlarmEnableSetting::displayString()
{
    static char alarmEnableString[10];
    bool enable = settingsList[0].get();
    snprintf(alarmEnableString, 10, "ALARM %s", enable ? "SET" : "OFF");
    return std::string(alarmEnableString, 9);
}

bool AlarmEnableSetting::getInput(const Input &input)
{
    bool enable = static_cast<bool>(settingsList[0].get());
    settingsList[0].set(!enable);
    SetAlarm(!enable);
    return true;
}

TemperatureSetting::TemperatureSetting()
{
    Setting units = Setting("units", 0, 1, 0);
    addSetting(units);
}

std::string TemperatureSetting::displayString()
{
    static const size_t TempStringSize = 3;
    constexpr uint8_t DEGREE_SYMBOL = 223;
    static char tempString[TempStringSize];
    bool degrees = settingsList[0].get();
    if (degrees)
    {
        snprintf(tempString, TempStringSize, "%cF", DEGREE_SYMBOL);
    }
    else
    {
        snprintf(tempString, TempStringSize, "%cC", DEGREE_SYMBOL);
    }
    return std::string(tempString, 2);
}

bool TemperatureSetting::getInput(const Input &input)
{
    bool units = static_cast<bool>(settingsList[0].get());
    settingsList[0].set(!units);
    return true;
}

BacklightSetting::BacklightSetting()
{
    Setting red = Setting("red", 0, 255, 0);
    Setting green = Setting("green", 0, 255, 0);
    Setting blue = Setting("blue", 0, 255, 0);

    addSetting(red);
    addSetting(green);
    addSetting(blue);
}

std::string BacklightSetting::displayString()
{
    static const size_t BackLightStringSize = 19;
    static char backlightString[BackLightStringSize];
    const int r = settingsList[0].get();
    const int g = settingsList[1].get();
    const int b = settingsList[2].get();
    snprintf(backlightString, BackLightStringSize, "R %3d G %3d B %3d", r, g, b);
    return std::string(backlightString, BackLightStringSize - 1);
}

bool BacklightSetting::getInput(const Input &input)
{
    return false;
}

ConstrastSetting::ConstrastSetting()
{
    Setting contrast = Setting("constrast", 0, 255, 0);

    addSetting(contrast);
}

std::string ConstrastSetting::displayString()
{
    static const size_t ConstrastStringSize = 14;
    static char constrastString[ConstrastStringSize];
    const int contrast = settingsList[0].get();
    snprintf(constrastString, ConstrastStringSize, "Contrast: %3d", contrast);
    return std::string(constrastString, ConstrastStringSize - 1);
}

bool ConstrastSetting::getInput(const Input &input)
{
    static int entriesToEdit = 1;
    if (input == Input::UP)
    {
        if (entriesToEdit == 1)
        {
            settingsList[2].increment();
        }
    }
    else if (input == Input::DOWN)
    {
        if (entriesToEdit == 1)
        {
            settingsList[2].decrement();
        }
    }
    else if (input == Input::ENTER)
    {
        entriesToEdit--;
        if (entriesToEdit == 0)
        {
            uint8_t contrast = settingsList[0].get();
            SetContrast(contrast);
            entriesToEdit = 1;
            return true;
        }
    }
    return false;
}