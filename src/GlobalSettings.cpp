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