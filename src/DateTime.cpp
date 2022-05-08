#include "DateTime.h"


uint8_t calculateMaxDayOfMonth(uint8_t month, uint8_t year)
{
    // Credit to : http://www.codecodex.com/wiki/Calculate_the_number_of_days_in_a_month
    // This protects against invalid months
    int numberOfDays;
    if (month == 4 || month == 6 || month == 9 || month == 11)
        numberOfDays = 30;
    else if (month == 2)
    {
        bool isLeapYear = (((year % 4) == 0) && ((year % 100) != 0)) || ((year % 400) == 0);
        if (isLeapYear)
            numberOfDays = 29;
        else
            numberOfDays = 28;
    }
    else
        numberOfDays = 31;

    return numberOfDays;
};

void ConvertTo12Hours(uint8_t hour24_in, uint8_t &hours12Out, bool &PM_notAMOut)
{
    if (hour24_in >= 12)
    {
        hours12Out = hour24_in - 12;
        PM_notAMOut = true;
    }
    else
    {
        hours12Out = hour24_in;
        PM_notAMOut = false;
    }
    if (hours12Out == 0)
        hours12Out = 12;
};
