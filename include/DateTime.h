#pragma once

#include <string>
#include <sstream>
#include <iomanip>

// Credit to SparkFun Library for the Build Dates: https://github.com/sparkfun/SparkFun_DS3234_RTC_Arduino_Library
// Parse the __DATE__ predefined macro to generate date defaults:
// __Date__ Format: MMM DD YYYY (First D may be a space if <10)
// <MONTH>
constexpr int BUILD_MONTH_JAN = ((__DATE__[0] == 'J') && (__DATE__[1] == 'a')) ? 1 : 0;
constexpr int BUILD_MONTH_FEB = (__DATE__[0] == 'F') ? 2 : 0;
constexpr int BUILD_MONTH_MAR = ((__DATE__[0] == 'M') && (__DATE__[1] == 'a') && (__DATE__[2] == 'r')) ? 3 : 0;
constexpr int BUILD_MONTH_APR = ((__DATE__[0] == 'A') && (__DATE__[1] == 'p')) ? 4 : 0;
constexpr int BUILD_MONTH_MAY = ((__DATE__[0] == 'M') && (__DATE__[1] == 'a') && (__DATE__[2] == 'y')) ? 5 : 0;
constexpr int BUILD_MONTH_JUN = ((__DATE__[0] == 'J') && (__DATE__[1] == 'u') && (__DATE__[2] == 'n')) ? 6 : 0;
constexpr int BUILD_MONTH_JUL = ((__DATE__[0] == 'J') && (__DATE__[1] == 'u') && (__DATE__[2] == 'l')) ? 7 : 0;
constexpr int BUILD_MONTH_AUG = ((__DATE__[0] == 'A') && (__DATE__[1] == 'u')) ? 8 : 0;
constexpr int BUILD_MONTH_SEP = (__DATE__[0] == 'S') ? 9 : 0;
constexpr int BUILD_MONTH_OCT = (__DATE__[0] == 'O') ? 10 : 0;
constexpr int BUILD_MONTH_NOV = (__DATE__[0] == 'N') ? 11 : 0;
constexpr int BUILD_MONTH_DEC = (__DATE__[0] == 'D') ? 12 : 0;
constexpr int BUILD_MONTH = BUILD_MONTH_JAN | BUILD_MONTH_FEB | BUILD_MONTH_MAR |
                            BUILD_MONTH_APR | BUILD_MONTH_MAY | BUILD_MONTH_JUN |
                            BUILD_MONTH_JUL | BUILD_MONTH_AUG | BUILD_MONTH_SEP |
                            BUILD_MONTH_OCT | BUILD_MONTH_NOV | BUILD_MONTH_DEC;
// <DATE>
constexpr int BUILD_DATE_0 = ((__DATE__[4] == ' ') ? 0 : (__DATE__[4] - 0x30));
constexpr int BUILD_DATE_1 = (__DATE__[5] - 0x30);
constexpr int BUILD_DATE = ((BUILD_DATE_0 * 10) + BUILD_DATE_1);
// <YEAR>
constexpr int BUILD_YEAR = (((__DATE__[7] - 0x30) * 1000) + ((__DATE__[8] - 0x30) * 100) +
                            ((__DATE__[9] - 0x30) * 10) + ((__DATE__[10] - 0x30) * 1));

// Parse the __TIME__ predefined macro to generate time defaults:
// __TIME__ Format: HH:MM:SS (First number of each is padded by 0 if <10)
// <HOUR>
constexpr int BUILD_HOUR_0 = ((__TIME__[0] == ' ') ? 0 : (__TIME__[0] - 0x30));
constexpr int BUILD_HOUR_1 = (__TIME__[1] - 0x30);
constexpr int BUILD_HOUR = ((BUILD_HOUR_0 * 10) + BUILD_HOUR_1);
// <MINUTE>
constexpr int BUILD_MINUTE_0 = ((__TIME__[3] == ' ') ? 0 : (__TIME__[3] - 0x30));
constexpr int BUILD_MINUTE_1 = (__TIME__[4] - 0x30);
constexpr int BUILD_MINUTE((BUILD_MINUTE_0 * 10) + BUILD_MINUTE_1);
// <SECOND>
constexpr int BUILD_SECOND_0 = ((__TIME__[6] == ' ') ? 0 : (__TIME__[6] - 0x30));
constexpr int BUILD_SECOND_1 = (__TIME__[7] - 0x30);
constexpr int BUILD_SECOND = ((BUILD_SECOND_0 * 10) + BUILD_SECOND_1);

enum DAYS
{
    SUNDAY = 1,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY,
};

struct DATE_TIME
{
    uint8_t year;
    uint8_t month;
    uint8_t dayofMonth;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    bool hour12_not24;
    bool PM_notAM;

    std::string DateToString()
    {
        std::stringstream ss;
        // date is day of month
        ss << std::setfill('0') << std::setw(2) << (int)month << "/";
        ss << std::setfill('0') << std::setw(2) << (int)dayofMonth << "/";
        ss << (int)(year + 2000);

        return ss.str();
    }

    std::string TimeToString()
    {
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(2) << (int)hour << ":";
        ss << std::setfill('0') << std::setw(2) << (int)minute << ":";
        ss << std::setfill('0') << std::setw(2) << (int)second;
        if (hour12_not24)
        {
            ss << " " << (PM_notAM ? "PM" : "AM");
        }

        return ss.str();
    }
};

uint8_t calculateMaxDayOfMonth(uint8_t month, uint8_t year);

void ConvertTo12Hours(uint8_t hour24_in, uint8_t &hours12Out, bool &PM_notAMOut);