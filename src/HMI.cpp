#include "HMI.h"

#include "esp_log.h"

#include "DeviceCommands.h"

static char LCD_BCKL_COLORS[COLOR_COUNT][4] = {
    "RED",
    "GRN",
    "BLU",
    "FUL",
    "MED",
    "LOW",
};

static char settingNames[SETTINGS_COUNT][5] = {
    "DATE",
    "TIME",
    "TEMP",
    "CNTR",
    "BLKT",
};

static const size_t backlightStringSize = 15;
static const size_t DateStringSize = 15;
static const size_t TimeStringSize = 15;
static const size_t contrastStringSize = 14;
static const size_t TempStringSize = 15;

static char dateString[DateStringSize];
static char timeString[TimeStringSize];
static char backlightString[backlightStringSize];
static char contrastString[contrastStringSize];

static uint8_t timeRow = 0;
static uint8_t timeCol = 0;

static uint8_t dateRow = 1;
static uint8_t dateCol = 0;

static uint8_t tempRow = 0;
static uint8_t tempCol = 12;

static uint8_t setnRow = 2;
static uint8_t setnCol = 0;

static uint8_t altSetnRow = 3;
static uint8_t altSetnCol = 0;

// Public
HMI::HMI()
{
    lcd.ResetCursor();
    lcd.DisableSystemMessages();
    lcd.Display();
    lcd.SetContrast(0);
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

    settingMode.max_value = (int)SETTINGS_COUNT - 1;
    settingMode.min_value = 0;

    contrastSetting.max_value = 255;
    contrastSetting.min_value = 0;
    contrastSetting.value = 0;

    backlightSetting.max_value = (int)COLOR_COUNT - 1;
    backlightSetting.min_value = 0;
    backlightSetting.value = 0;

    backLightValues[0] = {255, 0, 0};
    backLightValues[1] = {0, 255, 0};
    backLightValues[2] = {0, 0, 255};
    backLightValues[3] = {255, 255, 255};
    backLightValues[4] = {128, 128, 128};
    backLightValues[5] = {0, 0, 0};
}

void HMI::Reset()
{
    lcd.Reset();
}

void HMI::Process()
{

    switch (displayState)
    {
    case DISPLAYING:
        DisplayMode();
        break;
    case EDITING:
        EditMode();
        break;
    default:
        break;
    }
}

void HMI::SetDisplayTemperature(float temperatureF, float temperatureC)
{
    this->temperatureF = temperatureF;
    this->temperatureC = temperatureC;
}

void HMI::SetDisplayDateTime(DATE_TIME &dateTime)
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

HMIState HMI::GetCurrentState()
{
    return displayState;
}

// Private
void HMI::DisplayMode()
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
            UpdateDisplay();
            break;
        case LCD_RESET:
            lcd.Reset();
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
                DisplayContrast();
                break;
            case SETTING_BACKLIGHT:
                entriesToEdit = 1;
                DisplayBacklight();
                break;
            default:
                break;
            }
            DisplayCurrentState();
        }
        else if (msg.id == ALT_BTN_PRESSED)
        {
            static bool displayActive = true;
            if (displayActive)
            {
                lcd.NoDisplay();
                lcd.SetBackLightFast(0, 0, 0);
            }
            else
            {
                lcd.Display();
                int index = backlightSetting.value;
                uint8_t r = backLightValues[index].r;
                uint8_t g = backLightValues[index].g;
                uint8_t b = backLightValues[index].b;
                setBackLight(r, g, b);
            }
            displayActive = !displayActive;
        }
        else if (msg.id == UP_PRESSED || msg.id == DOWN_PRESSED)
        {
            settingMode.adjust(msg.id == UP_PRESSED);
            DisplayCurrentState();
        }
    }
}

void HMI::DisplayDate()
{
    // Date Update
    snprintf(dateString, DateStringSize, "%02d/%02d/%d",
             (uint8_t)dateSetting.month.value,
             (uint8_t)dateSetting.dayOfMonth.value,
             (uint16_t)dateSetting.year.value + 2000);
    lcd.SetCursor(dateRow, dateCol);
    lcd.WriteCharacters(dateString, 10);
}

void HMI::DisplayTime()
{
    // Time update
    lcd.SetCursor(timeRow, timeCol);
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

void HMI::DisplayTemperature()
{
    static char tempString[TempStringSize];
    lcd.SetCursor(tempRow, tempCol);
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

void HMI::DisplayCurrentState()
{
    lcd.SetCursor(setnRow, setnCol);
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
    lcd.WriteCharacters(settingNames[settingMode.value], 4);
}

void HMI::UpdateDisplay()
{
    DisplayDate();
    DisplayTime();
    DisplayTemperature();
    DisplayCurrentState();
}

void HMI::DisplayContrast()
{
    lcd.SetCursor(altSetnRow, altSetnCol);
    snprintf(contrastString, contrastStringSize, "Contrast: %3d", contrastSetting.value);
    lcd.WriteCharacters(contrastString, contrastStringSize - 1);
}

void HMI::DisplayBacklight()
{
    lcd.SetCursor(altSetnRow, altSetnCol);
    snprintf(backlightString, backlightStringSize, "Backlight: %s", LCD_BCKL_COLORS[backlightSetting.value]);
    lcd.WriteCharacters(backlightString, backlightStringSize - 1);
}

void HMI::EditMode()
{
    switch (settingMode.value)
    {
    case SETTING_DATE:
        EditingDate();
        break;
    case SETTING_TIME:
        EditingTime();
        break;
    case SETTING_TEMP:
        ChangeTemp();
        break;
    case SETTING_CONTRAST:
        EditContrast();
        break;
    case SETTING_BACKLIGHT:
        EditBackLight();
        break;
    default:
        break;
    }
}

void HMI::EditingDate()
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
            DisplayDate();
        }
        else if (msg.id == EDIT_MODE_PRESSED)
        {
            entriesToEdit--;
            if (entriesToEdit == 0)
            {
                setDate(dateSetting.dayOfMonth.value, dateSetting.month.value, dateSetting.year.value);
                displayState = DISPLAYING;
                DisplayCurrentState();
            }
        }
    }
}

void HMI::EditingTime()
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
            DisplayTime();
        }
        else if (msg.id == EDIT_MODE_PRESSED)
        {
            entriesToEdit--;
            if (entriesToEdit == 0)
            {
                setTime(timeSetting.hour.value, timeSetting.minute.value, timeSetting.second.value);
                displayState = DISPLAYING;
                DisplayCurrentState();
            }
        }
    }
}

void HMI::ChangeTemp()
{
    tempSetting.adjust(!tempSetting.value);
    displayState = DISPLAYING;
    DisplayTemperature();
    DisplayCurrentState();
}

void HMI::EditContrast()
{
    COMMAND_MESSAGE msg;
    if (recieveButtonCommand(&msg))
    {
        if (msg.id == UP_PRESSED || msg.id == DOWN_PRESSED)
        {
            bool increase = msg.id == UP_PRESSED;
            contrastSetting.adjust(increase);
            DisplayContrast();
        }
        else if (msg.id == EDIT_MODE_PRESSED)
        {
            entriesToEdit--;
            if (entriesToEdit == 0)
            {
                setContrast(contrastSetting.value);
                displayState = DISPLAYING;
                lcd.ClearRow(3);
                DisplayCurrentState();
            }
        }
    }
}
void HMI::EditBackLight()
{
    COMMAND_MESSAGE msg;
    if (recieveButtonCommand(&msg))
    {
        if (msg.id == UP_PRESSED || msg.id == DOWN_PRESSED)
        {
            bool increase = msg.id == UP_PRESSED;
            backlightSetting.adjust(increase);
            DisplayBacklight();
        }
        else if (msg.id == EDIT_MODE_PRESSED)
        {
            entriesToEdit--;
            if (entriesToEdit == 0)
            {
                int index = backlightSetting.value;
                uint8_t r = backLightValues[index].r;
                uint8_t g = backLightValues[index].g;
                uint8_t b = backLightValues[index].b;
                setBackLight(r, g, b);
                displayState = DISPLAYING;
                lcd.ClearRow(3);
                DisplayCurrentState();
            }
        }
    }
}
