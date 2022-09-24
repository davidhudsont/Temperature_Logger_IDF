#include "GlobalSettings.h"

#include "DeviceCommands.h"
#include "DateTime.h"
#include <cmath>

DateSetting::DateSetting()
    : Settings(3)
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
    if (input == Input::UP)
    {
        settingsList[currentEntry].increment();
    }
    else if (input == Input::DOWN)
    {
        settingsList[currentEntry].decrement();
    }
    else if (input == Input::ENTER)
    {
        currentEntry++;
        if (currentEntry == maxEntries - 1)
        {
            uint8_t year = settingsList[0].get();
            uint8_t month = settingsList[1].get();
            uint8_t max_value = calculateMaxDayOfMonth(month, year);
            settingsList[2].set_max(max_value);
        }
        if (currentEntry == maxEntries)
        {
            uint8_t year = settingsList[0].get();
            uint8_t month = settingsList[1].get();
            uint8_t dayOfMonth = settingsList[2].get();
            SetDate(dayOfMonth, month, year);
            currentEntry = 0;
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
    : Settings(3)
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
    if (input == Input::UP)
    {
        settingsList[currentEntry].increment();
    }
    else if (input == Input::DOWN)
    {
        settingsList[currentEntry].decrement();
    }
    else if (input == Input::ENTER)
    {
        currentEntry++;
        if (currentEntry == maxEntries)
        {
            uint8_t hour = settingsList[0].get();
            uint8_t minute = settingsList[1].get();
            uint8_t amPm = settingsList[2].get();
            SetTime12(hour, minute, amPm);
            currentEntry = 0;
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
    : Settings(3)
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
    if (input == Input::UP)
    {
        settingsList[currentEntry].increment();
    }
    else if (input == Input::DOWN)
    {
        settingsList[currentEntry].decrement();
    }
    else if (input == Input::ENTER)
    {
        currentEntry++;
        if (currentEntry == maxEntries)
        {
            uint8_t hour = settingsList[0].get();
            uint8_t minute = settingsList[1].get();
            uint8_t amPm = settingsList[2].get();
            SetAlarmTime12(hour, minute, amPm);
            currentEntry = 0;
            return true;
        }
    }
    return false;
}

AlarmEnableSetting::AlarmEnableSetting()
    : Settings(1)
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
    if (input == Input::UP)
    {
        settingsList[currentEntry].increment();
    }
    else if (input == Input::DOWN)
    {
        settingsList[currentEntry].decrement();
    }
    else if (input == Input::ENTER)
    {
        currentEntry++;
        if (currentEntry == maxEntries)
        {
            uint8_t enable = settingsList[0].get();
            SetAlarm(enable);
            currentEntry = 0;
            return true;
        }
    }
    return false;
}

TemperatureSetting::TemperatureSetting()
    : Settings(1)
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
    if (input == Input::UP)
    {
        settingsList[currentEntry].increment();
    }
    else if (input == Input::DOWN)
    {
        settingsList[currentEntry].decrement();
    }
    else if (input == Input::ENTER)
    {
        currentEntry++;
        if (currentEntry == maxEntries)
        {
            uint8_t contrast = settingsList[0].get();
            SetContrast(contrast);
            currentEntry = 0;
            return true;
        }
    }
    return false;
}

BacklightSetting::BacklightSetting()
    : Settings(3)
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
    : Settings(1)
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
    if (input == Input::UP)
    {
        settingsList[currentEntry].increment();
    }
    else if (input == Input::DOWN)
    {
        settingsList[currentEntry].decrement();
    }
    else if (input == Input::ENTER)
    {
        currentEntry++;
        if (currentEntry == maxEntries)
        {
            uint8_t contrast = settingsList[0].get();
            SetContrast(contrast);
            currentEntry = 0;
            return true;
        }
    }
    return false;
}

BacklightColorsSetting::BacklightColorsSetting()
    : Settings(3)
{
    auto rgbToInt = [](int r, int g, int b)
    {
        int color = 0;
        color = (r << 16) | (g << 8) | b;
        return color;
    };

    Setting red = Setting("RED", 0, 0x00FFFFFF, rgbToInt(255, 0, 0));
    Setting green = Setting("GRN", 0, 0x00FFFFFF, rgbToInt(0, 255, 0));
    Setting blue = Setting("BLU", 0, 0x00FFFFFF, rgbToInt(0, 0, 255));

    addSetting(red);
    addSetting(green);
    addSetting(blue);
}

std::string BacklightColorsSetting::displayString()
{
    static const size_t BacklightStringSize = 11;
    static char backlightString[BacklightStringSize];
    std::string color = settingsList[currentEntry].getName();
    snprintf(backlightString, BacklightStringSize, "Color: %s", color.c_str());
    return std::string(backlightString, BacklightStringSize - 1);
}

bool BacklightColorsSetting::getInput(const Input &input)
{
    if (input == Input::UP)
    {
        if (currentEntry == settingsList.size() - 1)
        {
            currentEntry = 0;
        }
        else
        {
            currentEntry++;
        }
    }
    else if (input == Input::DOWN)
    {
        if (currentEntry == 0)
        {
            currentEntry = settingsList.size() - 1;
        }
        else
        {
            currentEntry--;
        }
    }
    else if (input == Input::ENTER)
    {
        int rgb = settingsList[currentEntry].get();
        // convert from hex triplet to byte values
        uint8_t r = (rgb >> 16) & 0x0000FF;
        uint8_t g = (rgb >> 8) & 0x0000FF;
        uint8_t b = rgb & 0x0000FF;

        SetBackLight(r, g, b);
        return true;
    }
    return false;
}