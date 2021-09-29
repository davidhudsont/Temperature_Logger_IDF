#include "HMI.h"

#include <string>
#include <sstream>
#include <iomanip>

#include "esp_log.h"

#include "DeviceCommands.h"

HMI::HMI()
{
    lcd.Begin();
    lcd.ResetCursor();
    lcd.DisableSystemMessages();
    lcd.Display();
    lcd.SetBackLightFast(125, 125, 125);
}

void HMI::displayCurrentState()
{
    lcd.SetCursor(3, 0);
    switch (displayState)
    {
    case DISPLAYING:
        lcd.WriteCharacters("DISP ", 5);
        break;
    case EDITING:
        lcd.WriteCharacters("EDIT ", 5);
        break;
    }
    lcd.WriteCharacters("SETN: ", 6);
    switch (settingState)
    {
    case SETTING_DATE:
        lcd.WriteCharacters("DATE", 4);
        break;
    case SETTING_TIME:
        lcd.WriteCharacters("TIME", 4);
        break;
    case SETTING_TEMP:
        lcd.WriteCharacters("TEMP", 4);
        break;
    }
}

void HMI::display()
{
    COMMAND_MESSAGE msg;
    if (recieveLCDCommand(&msg))
    {
        switch (msg.id)
        {
        case LCD_DISPLAY_ON:
            ESP_LOGI("LCD", "DISPLAY ON");
            lcd.SetBackLightFast(125, 125, 125);
            lcd.Display();
            break;
        case LCD_DISPLAY_OFF:
            ESP_LOGI("LCD", "DISPLAY OFF");
            lcd.SetBackLightFast(0, 0, 0);
            lcd.NoDisplay();
            break;
        case LCD_SET_CONTRAST:
            ESP_LOGI("LCD", "Set Contrast %d", msg.arg1);
            lcd.SetContrast(msg.arg1);
            break;
        case LCD_SET_BACKLIGHT:
            ESP_LOGI("LCD", "Set Backlight r %d, g %d, b %d", msg.arg1, msg.arg2, msg.arg3);
            lcd.SetBackLightFast(msg.arg1, msg.arg2, msg.arg3);
            break;
        case LCD_CLEAR_DISPLAY:
            lcd.Clear();
            break;
        case LCD_DISPLAY_UPDATE:
            update();
            break;
        default:
            break;
        }
    }
    else if (recieveButtonCommand(&msg))
    {
        if (msg.id == EDIT_MODE_PRESSED)
        {
            displayState = EDITING;
            ESP_LOGI("BTN", "Display Mode State: %d", displayState);
            switch (settingState)
            {
            case SETTING_DATE:
                entriesToEdit = 3;
                break;
            case SETTING_TIME:
                entriesToEdit = 3;
                break;
            case SETTING_TEMP:
                entriesToEdit = 1;
                break;
            default:
                break;
            }
            displayCurrentState();
        }
        else if (msg.id == UP_PRESSED)
        {
            switch (settingState)
            {
            case SETTING_DATE:
                settingState = SETTING_TIME;
                break;
            case SETTING_TIME:
                settingState = SETTING_TEMP;
                break;
            case SETTING_TEMP:
                settingState = SETTING_DATE;
                break;
            }
            displayCurrentState();
        }
        else if (msg.id == DOWN_PRESSED)
        {
            switch (settingState)
            {
            case SETTING_DATE:
                settingState = SETTING_TEMP;
                break;
            case SETTING_TIME:
                settingState = SETTING_DATE;
                break;
            case SETTING_TEMP:
                settingState = SETTING_TIME;
                break;
            }
            displayCurrentState();
        }
    }
}

void HMI::editHour(bool increase)
{
    dateTime.hour = increase ? dateTime.hour + 1 : dateTime.hour - 1;
    if (dateTime.hour > 23)
        dateTime.hour = 0;
    else if (dateTime.hour == 0)
        dateTime.hour = 23;
    lcd.SetCursor(1, 0);
    ESP_LOGI("HMI", "Hour %d", dateTime.hour);
    lcd.WriteDigit(dateTime.hour / 10);
    lcd.WriteDigit(dateTime.hour % 10);
}

void HMI::editMinute(bool increase)
{
    dateTime.minute = increase ? dateTime.minute + 1 : dateTime.minute - 1;
    if (dateTime.minute > 59)
        dateTime.minute = 0;
    else if (dateTime.minute == 0)
        dateTime.minute = 59;
    lcd.SetCursor(1, 3);
    ESP_LOGI("HMI", "Minute %d", dateTime.minute);
    lcd.WriteDigit(dateTime.minute / 10);
    lcd.WriteDigit(dateTime.minute % 10);
}

void HMI::editSecond(bool increase)
{
    dateTime.second = increase ? dateTime.second + 1 : dateTime.second - 1;
    if (dateTime.second > 59)
        dateTime.second = 0;
    else if (dateTime.second == 0)
        dateTime.second = 59;
    lcd.SetCursor(1, 6);
    ESP_LOGI("HMI", "Second %d", dateTime.second);
    lcd.WriteDigit(dateTime.second / 10);
    lcd.WriteDigit(dateTime.second % 10);
}

void HMI::editingTime()
{
    COMMAND_MESSAGE msg;
    if (recieveButtonCommand(&msg))
    {
        if (entriesToEdit == 3)
        {
            if (msg.id == UP_PRESSED)
            {
                editHour(true);
            }
            else if (msg.id == DOWN_PRESSED)
            {
                editHour(false);
            }
            else if (msg.id == EDIT_MODE_PRESSED)
            {
                --entriesToEdit;
                setHours24Mode(dateTime.hour);
            }
        }
        else if (entriesToEdit == 2)
        {
            if (msg.id == UP_PRESSED)
            {
                editMinute(true);
            }
            else if (msg.id == DOWN_PRESSED)
            {
                editMinute(false);
            }
            else if (msg.id == EDIT_MODE_PRESSED)
            {
                --entriesToEdit;
                setMinutes(dateTime.minute);
            }
        }
        else if (entriesToEdit == 1)
        {
            if (msg.id == UP_PRESSED)
            {
                editSecond(true);
            }
            else if (msg.id == DOWN_PRESSED)
            {
                editSecond(false);
            }
            else if (msg.id == EDIT_MODE_PRESSED)
            {
                displayState = DISPLAYING;
                displayCurrentState();
                setSeconds(dateTime.second);
            }
        }
    }
}

void HMI::editMonth(bool increase)
{
    dateTime.month = increase ? dateTime.month + 1 : dateTime.month - 1;
    if (dateTime.month > 12)
        dateTime.month = 1;
    else if (dateTime.month == 0)
        dateTime.month = 12;
    lcd.SetCursor(0, 0);
    ESP_LOGI("HMI", "Month %d", dateTime.month);
    lcd.WriteDigit(dateTime.month / 10);
    lcd.WriteDigit(dateTime.month % 10);
}

void HMI::editDayOfMonth(bool increase)
{

    dateTime.dayofMonth = increase ? dateTime.dayofMonth + 1 : dateTime.dayofMonth - 1;
    uint8_t maxDaysOfMonth = calculateMaxDayOfMonth(dateTime.month, dateTime.year);
    if (dateTime.dayofMonth > maxDaysOfMonth)
        dateTime.dayofMonth = 1;
    else if (dateTime.dayofMonth == 0)
        dateTime.dayofMonth = maxDaysOfMonth;
    lcd.SetCursor(0, 3);
    ESP_LOGI("HMI", "Day Of Month %d", dateTime.dayofMonth);
    lcd.WriteDigit(dateTime.dayofMonth / 10);
    lcd.WriteDigit(dateTime.dayofMonth % 10);
}

void HMI::editYear(bool increase)
{
    dateTime.year = increase ? dateTime.year + 1 : dateTime.year - 1;
    if (dateTime.year == 0)
        dateTime.year = 0;
    lcd.SetCursor(0, 6);
    ESP_LOGI("HMI", "Year %d", dateTime.year);
    lcd.WriteDigit(2);
    lcd.WriteDigit(0);
    lcd.WriteDigit(dateTime.year / 10);
    lcd.WriteDigit(dateTime.year % 10);
}

void HMI::editingDate()
{
    COMMAND_MESSAGE msg;
    if (recieveButtonCommand(&msg))
    {
        if (entriesToEdit == 3)
        {
            if (msg.id == UP_PRESSED)
            {
                editMonth(true);
            }
            else if (msg.id == DOWN_PRESSED)
            {
                editMonth(false);
            }
            else if (msg.id == EDIT_MODE_PRESSED)
            {
                --entriesToEdit;
                setMonth(dateTime.month);
            }
        }
        else if (entriesToEdit == 2)
        {
            if (msg.id == UP_PRESSED)
            {
                editDayOfMonth(true);
            }
            else if (msg.id == DOWN_PRESSED)
            {
                editDayOfMonth(false);
            }
            else if (msg.id == EDIT_MODE_PRESSED)
            {
                --entriesToEdit;
                setDayOfMonth(dateTime.dayofMonth);
            }
        }
        else if (entriesToEdit == 1)
        {
            if (msg.id == UP_PRESSED)
            {
                editYear(true);
            }
            else if (msg.id == DOWN_PRESSED)
            {
                editYear(false);
            }
            else if (msg.id == EDIT_MODE_PRESSED)
            {
                displayState = DISPLAYING;
                displayCurrentState();
                setYear(dateTime.year);
            }
        }
    }
}

void HMI::changeTemp()
{
    displayTempF_notC = !displayTempF_notC;
    ESP_LOGI("HMI", "Changed Tempearture Display Units");
    displayState = DISPLAYING;
    lcd.ClearRow(2);
    std::stringstream ss3;
    if (displayTempF_notC)
    {
        ss3 << std::setprecision(5) << temperatureF << DEGREE_SYMBOL << "F";
    }
    else
    {
        ss3 << std::setprecision(5) << temperatureC << DEGREE_SYMBOL << "C";
    }

    std::string logtemp = ss3.str();
    lcd.SetCursor(2, 0);
    lcd.WriteCharacters(logtemp.c_str(), logtemp.length());

    displayCurrentState();
}

void HMI::editing()
{
    switch (settingState)
    {
    case SETTING_DATE:
        editingDate();
        break;
    case SETTING_TIME:
        editingTime();
        break;
    case SETTING_TEMP:
        changeTemp();
        break;
    default:
        break;
    }
}

void HMI::update()
{

    // Date Update
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << (int)dateTime.month << "/";
    ss << std::setfill('0') << std::setw(2) << (int)dateTime.dayofMonth << "/";
    ss << (int)(dateTime.year + 2000);
    std::string logdate = ss.str();
    lcd.SetCursor(0, 0);
    lcd.WriteCharacters(logdate.c_str(), logdate.length());

    // Time update
    std::stringstream ss2;
    ss2 << std::setfill('0') << std::setw(2) << (int)dateTime.hour << ":";
    ss2 << std::setfill('0') << std::setw(2) << (int)dateTime.minute << ":";
    ss2 << std::setfill('0') << std::setw(2) << (int)dateTime.second;
    if (dateTime.hour12_not24)
    {
        ss2 << " " << (dateTime.PM_notAM ? "PM" : "AM");
    }
    std::string logtime = ss2.str();
    lcd.SetCursor(1, 0);
    lcd.WriteCharacters(logtime.c_str(), logtime.length());

    std::stringstream ss3;
    if (displayTempF_notC)
    {
        ss3 << std::setprecision(5) << temperatureF << DEGREE_SYMBOL << "F";
    }
    else
    {
        ss3 << std::setprecision(5) << temperatureC << DEGREE_SYMBOL << "C";
    }

    std::string logtemp = ss3.str();
    lcd.SetCursor(2, 0);
    lcd.WriteCharacters(logtemp.c_str(), logtemp.length());

    displayCurrentState();
}

void HMI::process()
{

    switch (displayState)
    {
    case DISPLAYING:
        display();
        break;
    case EDITING:
        editing();
        break;
    default:
        break;
    }
}

void HMI::setDisplayTemperature(float temperatureF, float temperatureC)
{
    this->temperatureF = temperatureF;
    this->temperatureC = temperatureC;
}

void HMI::setDisplayDateTime(DATE_TIME &dateTime)
{
    this->dateTime = dateTime;
}

HMIState HMI::getCurrentState()
{
    return displayState;
}
