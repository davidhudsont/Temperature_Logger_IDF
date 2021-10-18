#include "HMI.h"

#include "esp_log.h"

#include "DeviceCommands.h"

// Public
HMI::HMI()
{
    lcd.Begin();
    lcd.ResetCursor();
    lcd.DisableSystemMessages();
    lcd.Display();
    lcd.SetBackLightFast(125, 125, 125);

    dateSetting.month.max_value = 12;
    dateSetting.month.min_value = 1;
    dateSetting.dayOfMonth.max_value = 31;
    dateSetting.dayOfMonth.min_value = 1;
    dateSetting.year.max_value = 256;
    dateSetting.year.min_value = 0;

    timeSetting.hour.max_value = 23;
    timeSetting.hour.min_value = 0;
    timeSetting.minute.max_value = 59;
    timeSetting.minute.min_value = 0;
    timeSetting.second.max_value = 59;
    timeSetting.second.min_value = 0;

    tempSetting.max_value = 1;
    tempSetting.min_value = 0;
}

void HMI::process()
{

    switch (displayState)
    {
    case DISPLAYING:
        displayMode();
        break;
    case EDITING:
        editMode();
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
    dateSetting.month.value = dateTime.month;
    dateSetting.dayOfMonth.value = dateTime.dayofMonth;
    dateSetting.year.value = dateTime.year;

    timeSetting.hour.value = dateTime.hour;
    timeSetting.minute.value = dateTime.minute;
    timeSetting.second.value = dateTime.second;
}

HMIState HMI::getCurrentState()
{
    return displayState;
}

// Private
void HMI::displayMode()
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
            updateDisplay();
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
        else if (msg.id == UP_PRESSED || msg.id == DOWN_PRESSED)
        {
            int settingStateInt = msg.id == UP_PRESSED ? (int)settingState + 1 : (int)settingState - 1;
            if (settingStateInt >= (int)SETTINGS_COUNT)
            {
                settingStateInt = 0;
            }
            else if (settingStateInt < 0)
            {
                settingStateInt = (int)SETTINGS_COUNT - 1;
            }
            settingState = (HMISettings)settingStateInt;
            displayCurrentState();
        }
    }
}

void HMI::displayDate()
{
    // Date Update
    static const size_t DateStringSize = 15;
    static char dateString[DateStringSize];
    snprintf(dateString, DateStringSize, "%02d/%02d/%d",
             (uint8_t)dateSetting.month.value,
             (uint8_t)dateSetting.dayOfMonth.value,
             (uint16_t)dateSetting.year.value + 2000);
    lcd.SetCursor(0, 0);
    lcd.WriteCharacters(dateString, 10);
}

void HMI::displayTime()
{
    // Time update
    static const size_t TimeStringSize = 15;
    static char timeString[TimeStringSize];
    lcd.SetCursor(1, 0);
    if (dateTime.hour12_not24)
    {
        snprintf(timeString, TimeStringSize, "%02d:%02d:%02d %s",
                 (uint8_t)timeSetting.hour.value,
                 (uint8_t)timeSetting.minute.value,
                 (uint8_t)timeSetting.second.value,
                 (dateTime.PM_notAM ? "PM" : "AM"));
        lcd.WriteCharacters(timeString, 11);
    }
    else
    {
        snprintf(timeString, TimeStringSize, "%02d:%02d:%02d",
                 timeSetting.hour.value,
                 timeSetting.minute.value,
                 timeSetting.second.value);
        lcd.WriteCharacters(timeString, 8);
    }
}

void HMI::displayTemperature()
{
    static const size_t TempStringSize = 15;
    static char tempString[TempStringSize];
    lcd.SetCursor(2, 0);
    if (tempSetting.value)
    {
        if (temperatureF > 100)
            snprintf(tempString, TempStringSize, "%3.2f%cF", temperatureF, DEGREE_SYMBOL);
        else
            snprintf(tempString, TempStringSize, "%2.3f%cF", temperatureF, DEGREE_SYMBOL);
    }
    else
    {
        snprintf(tempString, TempStringSize, "%2.3f%cC", temperatureC, DEGREE_SYMBOL);
    }

    lcd.WriteCharacters(tempString, 8);
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
    default:
        break;
    }
}

void HMI::updateDisplay()
{
    displayDate();
    displayTime();
    displayTemperature();
    displayCurrentState();
}

void HMI::editMode()
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

void HMI::editingDate()
{
    COMMAND_MESSAGE msg;
    if (recieveButtonCommand(&msg))
    {
        if (msg.id == UP_PRESSED || msg.id == DOWN_PRESSED)
        {
            bool increase = msg.id == UP_PRESSED;
            if (entriesToEdit == 3)
            {
                dateSetting.month.adjust(increase);
            }
            else if (entriesToEdit == 2)
            {
                dateSetting.dayOfMonth.adjust(increase);
            }
            else if (entriesToEdit == 1)
            {
                dateSetting.year.adjust(increase);
            }
            displayDate();
        }
        else if (msg.id == EDIT_MODE_PRESSED)
        {
            entriesToEdit--;
            if (entriesToEdit == 0)
            {
                setMonth(dateTime.month);
                setDayOfMonth(dateTime.dayofMonth);
                setYear(dateTime.year);
                displayState = DISPLAYING;
                displayCurrentState();
            }
        }
    }
}

void HMI::editingTime()
{
    COMMAND_MESSAGE msg;
    if (recieveButtonCommand(&msg))
    {
        if (msg.id == UP_PRESSED || msg.id == DOWN_PRESSED)
        {
            bool increase = msg.id == UP_PRESSED;
            if (entriesToEdit == 3)
            {
                timeSetting.hour.adjust(increase);
            }
            else if (entriesToEdit == 2)
            {
                timeSetting.minute.adjust(increase);
            }
            else if (entriesToEdit == 1)
            {
                timeSetting.second.adjust(increase);
            }
            displayTime();
        }
        else if (msg.id == EDIT_MODE_PRESSED)
        {
            entriesToEdit--;
            if (entriesToEdit == 0)
            {
                setHours24Mode(dateTime.hour);
                setMinutes(dateTime.minute);
                setSeconds(dateTime.second);
                displayState = DISPLAYING;
                displayCurrentState();
            }
        }
    }
}

void HMI::changeTemp()
{
    tempSetting.adjust(!tempSetting.value);
    displayState = DISPLAYING;
    displayTemperature();
    displayCurrentState();
}
