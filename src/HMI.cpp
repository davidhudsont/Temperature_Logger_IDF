#include "HMI.h"
#include <string>
#include <sstream>
#include <iomanip>
#include "esp_log.h"

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
    std::string str = "DSP = ";
    lcd.WriteCharacters(str.c_str(), str.length());
    lcd.WriteCharacter((char)displayState + '0');

    str = " STN = ";
    lcd.WriteCharacters(str.c_str(), str.length());
    lcd.WriteCharacter((char)settingState + '0');
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
        if (msg.id == EDIT_SETTING_PRESSED)
        {
            switch (settingState)
            {
            case SETTING_DATE:
                settingState = SETTING_TIME;
                ESP_LOGI("BTN", "Setting Mode State: Time");
                break;
            case SETTING_TIME:
                settingState = SETTING_TEMP;
                ESP_LOGI("BTN", "Setting Mode State: Temp");
                break;
            case SETTING_TEMP:
                settingState = SETTING_DATE;
                ESP_LOGI("BTN", "Setting Mode State: Date");
                break;
            default:
                break;
            }
            displayCurrentState();
        }
    }
}

void HMI::editingTime()
{
    COMMAND_MESSAGE msg;
    if (recieveButtonCommand(&msg))
    {
        if (msg.id == EDIT_MODE_PRESSED)
        {
            displayState = DISPLAYING;
            displayCurrentState();
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
    if (dateTime.dayofMonth > 28)
        dateTime.dayofMonth = 1;
    else if (dateTime.dayofMonth == 0)
        dateTime.dayofMonth = 28;
    lcd.SetCursor(0, 3);
    ESP_LOGI("HMI", "Day Of Month %d", dateTime.dayofMonth);
    lcd.WriteDigit(dateTime.dayofMonth / 10);
    lcd.WriteDigit(dateTime.dayofMonth % 10);
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
            if (msg.id == DOWN_PRESSED)
            {
                editDayOfMonth(false);
            }
            if (msg.id == EDIT_MODE_PRESSED)
            {
                --entriesToEdit;
                setDayOfMonth(dateTime.dayofMonth);
            }
        }
        else if (entriesToEdit == 1)
        {
            if (msg.id == UP_PRESSED)
            {
                dateTime.year = dateTime.year + 1;
                lcd.SetCursor(0, 6);
                ESP_LOGI("HMI", "Year %d", dateTime.year);
                lcd.WriteDigit(2);
                lcd.WriteDigit(0);
                lcd.WriteDigit(dateTime.year / 10);
                lcd.WriteDigit(dateTime.year % 10);
            }
            if (msg.id == DOWN_PRESSED)
            {
                dateTime.year = dateTime.year - 1;
                if (dateTime.year == 0)
                    dateTime.year = 0;
                lcd.SetCursor(0, 6);
                ESP_LOGI("HMI", "Year %d", dateTime.year);
                lcd.WriteDigit(2);
                lcd.WriteDigit(0);
                lcd.WriteDigit(dateTime.year / 10);
                lcd.WriteDigit(dateTime.year % 10);
            }
            if (msg.id == EDIT_MODE_PRESSED)
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
