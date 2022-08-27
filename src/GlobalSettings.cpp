#include "GlobalSettings.h"

DateSetting::DateSetting()
{
    Setting year = Setting(0, 99, 0);
    Setting month = Setting(1, 12, 1);
    Setting dayOfMonth = Setting(1, 31, 1);

    addSetting("year", year);
    addSetting("month", month);
    addSetting("dayOfMonth", dayOfMonth);
}

std::string DateSetting::displayString()
{
    constexpr size_t dateStringSize = 15;
    static char dateString[dateStringSize] = "";

    uint8_t year = getSetting("year").get();
    uint8_t month = getSetting("month").get();
    uint8_t dayOfMonth = getSetting("dayOfMonth").get();

    snprintf(dateString, dateStringSize, "%02d/%02d/%d",
             month,
             dayOfMonth,
             year + 2000);

    return std::string(dateString, 10);
}

TimeSetting::TimeSetting()
{
    Setting hour = Setting(1, 12, 1);
    Setting minute = Setting(1, 59, 1);
    Setting amPm = Setting(0, 1, 0);

    addSetting("hour", hour);
    addSetting("minute", minute);
    addSetting("amPm", amPm);
}

std::string TimeSetting::displayString()
{
    constexpr size_t timeStringSize = 13;
    static char timeString[timeStringSize] = "";

    uint8_t hour = getSetting("hour").get();
    uint8_t minute = getSetting("minute").get();
    uint8_t amPm = getSetting("amPm").get();

    snprintf(timeString, timeStringSize, "%02d:%02d %s",
             hour,
             minute,
             amPm ? "PM" : "AM");
    return std::string(timeString, 8);
}

AlarmEnableSetting::AlarmEnableSetting()
{
    Setting enable = Setting(0, 1, 0);
    addSetting("enable", enable);
}

std::string AlarmEnableSetting::displayString()
{
    static char alarmEnableString[10];
    bool enable = getSetting("enable").get();
    snprintf(alarmEnableString, 10, "ALARM %s", enable ? "SET" : "OFF");
    return std::string(alarmEnableString, 9);
}

TemperatureSetting::TemperatureSetting()
{
    Setting units = Setting(0, 1, 0);
    addSetting("units", units);
}

std::string TemperatureSetting::displayString()
{
    static const size_t TempStringSize = 3;
    constexpr uint8_t DEGREE_SYMBOL = 223;
    static char tempString[TempStringSize];
    bool degrees = getSetting("units").get();
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

BacklightSetting::BacklightSetting()
{
    Setting red = Setting(0, 255, 0);
    Setting green = Setting(0, 255, 0);
    Setting blue = Setting(0, 255, 0);

    addSetting("red", red);
    addSetting("green", green);
    addSetting("blue", blue);
}

std::string BacklightSetting::displayString()
{
    static const size_t BackLightStringSize = 19;
    static char backlightString[BackLightStringSize];
    const int r = getSetting("red").get();
    const int g = getSetting("green").get();
    const int b = getSetting("blue").get();
    snprintf(backlightString, BackLightStringSize, "R %3d G %3d B %3d", r, g, b);
    return std::string(backlightString, BackLightStringSize - 1);
}