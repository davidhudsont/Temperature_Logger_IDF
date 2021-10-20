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
    dateSetting.year.max_value = 99;
    dateSetting.year.min_value = 0;

    timeSetting.hour.max_value = 23;
    timeSetting.hour.min_value = 0;
    timeSetting.minute.max_value = 59;
    timeSetting.minute.min_value = 0;
    timeSetting.second.max_value = 59;
    timeSetting.second.min_value = 0;

    tempSetting.max_value = 1;
    tempSetting.min_value = 0;

    settingMode.max_value = SETTINGS_COUNT - 1;
    settingMode.min_value = 0;

    contrastSetting.max_value = 255;
    contrastSetting.min_value = 0;
    contrastSetting.value = 0;

    backlightSetting.r.max_value = 255;
    backlightSetting.r.min_value = 0;
    backlightSetting.r.value = 125;

    backlightSetting.g.max_value = 255;
    backlightSetting.g.min_value = 0;
    backlightSetting.g.value = 125;

    backlightSetting.b.max_value = 255;
    backlightSetting.b.min_value = 0;
    backlightSetting.b.value = 125;
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
    dateSetting.month.value = dateTime.month;
    dateSetting.dayOfMonth.value = dateTime.dayofMonth;
    dateSetting.year.value = dateTime.year;

    timeSetting.hour.value = dateTime.hour;
    timeSetting.minute.value = dateTime.minute;
    timeSetting.second.value = dateTime.second;

    hour12_not24 = dateTime.hour12_not24;
    PM_notAM = dateTime.PM_notAM;
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
            switch (settingMode.value)
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
            case SETTING_CONTRAST:
                entriesToEdit = 1;
                break;
            case SETTING_BACKLIGHT:
                entriesToEdit = 3;
                break;
            default:
                break;
            }
            displayCurrentState();
        }
        else if (msg.id == UP_PRESSED || msg.id == DOWN_PRESSED)
        {
            settingMode.adjust(msg.id == UP_PRESSED);
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
    if (hour12_not24)
    {
        snprintf(timeString, TimeStringSize, "%02d:%02d:%02d %s",
                 (uint8_t)timeSetting.hour.value,
                 (uint8_t)timeSetting.minute.value,
                 (uint8_t)timeSetting.second.value,
                 (PM_notAM ? "PM" : "AM"));
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
    switch (settingMode.value)
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
    case SETTING_CONTRAST:
        lcd.WriteCharacters("CNTR", 4);
        break;
    case SETTING_BACKLIGHT:
        lcd.WriteCharacters("BKLT", 4);
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
    switch (settingMode.value)
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
    case SETTING_CONTRAST:
        editContrast();
        break;
    case SETTING_BACKLIGHT:
        editBackLight();
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
                dateSetting.dayOfMonth.max_value = calculateMaxDayOfMonth(dateSetting.month.value, dateSetting.year.value);
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
                setDate(dateSetting.dayOfMonth.value, dateSetting.month.value, dateSetting.year.value);
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
                setTime(timeSetting.hour.value, timeSetting.minute.value, timeSetting.second.value);
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

void HMI::editContrast()
{
    COMMAND_MESSAGE msg;
    static const size_t contrastStringSize = 14;
    static char contrastString[contrastStringSize];
    if (recieveButtonCommand(&msg))
    {
        if (msg.id == UP_PRESSED || msg.id == DOWN_PRESSED)
        {
            bool increase = msg.id == UP_PRESSED;
            contrastSetting.adjust(increase);
            lcd.SetCursor(2, 0);
            snprintf(contrastString, contrastStringSize, "Contrast: %3d", contrastSetting.value);
            lcd.WriteCharacters(contrastString, contrastStringSize);
        }
        else if (msg.id == EDIT_MODE_PRESSED)
        {
            entriesToEdit--;
            if (entriesToEdit == 0)
            {
                setContrast(contrastSetting.value);
                displayState = DISPLAYING;
                displayCurrentState();
            }
        }
    }
}
void HMI::editBackLight()
{
    COMMAND_MESSAGE msg;
    static const size_t backlightStringSize = 17;
    static char backlightString[backlightStringSize];
    if (recieveButtonCommand(&msg))
    {
        if (msg.id == UP_PRESSED || msg.id == DOWN_PRESSED)
        {
            bool increase = msg.id == UP_PRESSED;
            lcd.SetCursor(2, 0);
            if (entriesToEdit == 3)
            {
                backlightSetting.r.adjust(increase);
                snprintf(backlightString, backlightStringSize, "Backlight: r %3d", backlightSetting.r.value);
            }
            else if (entriesToEdit == 2)
            {
                backlightSetting.g.adjust(increase);
                snprintf(backlightString, backlightStringSize, "Backlight: g %3d", backlightSetting.g.value);
            }
            else if (entriesToEdit == 1)
            {
                backlightSetting.b.adjust(increase);
                snprintf(backlightString, backlightStringSize, "Backlight: b %3d", backlightSetting.b.value);
            }
            lcd.WriteCharacters(backlightString, backlightStringSize);
        }
        else if (msg.id == EDIT_MODE_PRESSED)
        {
            entriesToEdit--;
            if (entriesToEdit == 0)
            {
                setBackLight(backlightSetting.r.value, backlightSetting.g.value, backlightSetting.b.value);
                displayState = DISPLAYING;
                displayCurrentState();
            }
        }
    }
}
