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

    return std::string(dateString, 11);
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
    return std::string(timeString, 9);
}