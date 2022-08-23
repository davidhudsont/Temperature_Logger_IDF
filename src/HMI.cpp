
// RTOS
#include "esp_log.h"

// User Headers
#include "HMI.h"
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
    "ALRM",
    "ALRE",
};

static const size_t backlightStringSize = 15;
static const size_t DateStringSize = 15;
static const size_t TimeStringSize = 11;
static const size_t contrastStringSize = 14;
static const size_t TempStringSize = 15;
static const size_t alarmStringSize = 25;

static char dateString[DateStringSize];
static char timeString[TimeStringSize];
static char backlightString[backlightStringSize];
static char contrastString[contrastStringSize];
static char alarmSettingString[alarmStringSize];
static char alarmEnableString[20];

static uint8_t timeRow = 0;
static uint8_t timeCol = 0;

static uint8_t dateRow = 0;
static uint8_t dateCol = 10;

static uint8_t tempRow = 1;
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

    timeSetting.hour.max_value = 12;
    timeSetting.hour.min_value = 1;
    timeSetting.minute.max_value = 59;
    timeSetting.minute.min_value = 0;
    timeSetting.PM_notAM.max_value = 1;
    timeSetting.PM_notAM.min_value = 0;

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

    alarmSetting.hour.max_value = 24;
    alarmSetting.hour.min_value = 0;
    alarmSetting.minute.max_value = 60;
    alarmSetting.minute.min_value = 0;
    alarmSetting.PM_notAM.max_value = 1;
    alarmSetting.PM_notAM.min_value = 0;
    alarmSetting.enable.max_value = 1;
    alarmSetting.enable.min_value = 0;
    alarmSetting.enable.value = false;
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
    timeSetting.PM_notAM.value = dateTime.PM_notAM;

    hour12_not24 = dateTime.hour12_not24;
}

HMIState HMI::GetCurrentState()
{
    return displayState;
}

// Private
void HMI::DisplayMode()
{
    COMMAND_MESSAGE msg;
    if (RecieveLCDCommand(&msg))
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
    else if (EditButtonTakeSemaphore())
    {
        displayState = EDITING;
        ESP_LOGI("BTN", "Display Mode State: %d", displayState);
        switch (settingMode.value)
        {
        case SETTING_DATE:
            entriesToEdit = 3;
            DisplayCurrentState();
            break;
        case SETTING_TIME:
            entriesToEdit = 3;
            DisplayCurrentState();
            break;
        case SETTING_TEMP:
            entriesToEdit = 1;
            DisplayCurrentState();
            break;
        case SETTING_CONTRAST:
            entriesToEdit = 1;
            DisplayCurrentState();
            DisplayContrast();
            break;
        case SETTING_BACKLIGHT:
            entriesToEdit = 1;
            DisplayCurrentState();
            DisplayBacklight();
            break;
        case SETTING_ALARM:
            entriesToEdit = 3;
            DisplayCurrentState();
            DisplayAlarmSetting();
            break;
        case SETTING_ALARM_ENABLE:
            entriesToEdit = 3;
            DisplayCurrentState();
            DisplayAlarmEnable();
            break;
        default:
            break;
        }
    }
    else if (AltButtonTakeSemaphore())
    {
        SetAlarm(false);
    }
    else if (UpButtonTakeSemaphore())
    {
        settingMode.adjust(true);
        DisplayCurrentState();
    }
    else if (DownButtonTakeSemaphore())
    {
        settingMode.adjust(false);
        DisplayCurrentState();
    }
}

void HMI::DisplayAlarmEnable()
{
    bool alarmEnabled = alarmSetting.enable.value;
    snprintf(alarmEnableString, 10, "ALARM %s", alarmEnabled ? "SET" : "OFF");
    lcd.SetCursor(3, 0);
    lcd.WriteCharacters(alarmEnableString, 9);
}

void HMI::EditAlarmEnable()
{
    alarmSetting.enable.adjust(!alarmSetting.enable.value);
    SetAlarm(alarmSetting.enable.value);
    displayState = DISPLAYING;
    DisplayAlarmEnable();
    DisplayCurrentState();
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

    snprintf(timeString, TimeStringSize, "%02d:%02d %s",
             (uint8_t)timeSetting.hour.value,
             (uint8_t)timeSetting.minute.value,
             ((bool)timeSetting.PM_notAM.value ? "PM" : "AM"));
    lcd.WriteCharacters(timeString, 8);
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

void HMI::DisplayAlarmSetting()
{
    lcd.SetCursor(altSetnRow, altSetnCol);
    uint8_t hour = alarmSetting.hour.value;
    uint8_t minute = alarmSetting.minute.value;
    bool PM_notAM = alarmSetting.PM_notAM.value;
    snprintf(alarmSettingString, alarmStringSize, "Alarm: %02d:%02d %s", hour, minute, PM_notAM ? "PM" : "AM");
    lcd.WriteCharacters(alarmSettingString, 15);
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
    case SETTING_ALARM:
        EditAlarmTime();
        break;
    case SETTING_ALARM_ENABLE:
        EditAlarmEnable();
        break;
    default:
        break;
    }
}

void HMI::EditingDate()
{
    if (UpButtonTakeSemaphore())
    {
        bool increase = false;
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
    else if (DownButtonTakeSemaphore())
    {
        bool increase = true;
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
    else if (EditButtonTakeSemaphore())
    {
        entriesToEdit--;
        if (entriesToEdit == 0)
        {
            SetDate(dateSetting.dayOfMonth.value, dateSetting.month.value, dateSetting.year.value);
            displayState = DISPLAYING;
            DisplayCurrentState();
        }
    }
}

void HMI::EditingTime()
{
    if (UpButtonTakeSemaphore())
    {
        bool increase = true;
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
            timeSetting.PM_notAM.adjust(increase);
        }
        DisplayTime();
    }
    else if (DownButtonTakeSemaphore())
    {
        bool increase = false;
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
            timeSetting.PM_notAM.adjust(increase);
        }
        DisplayTime();
    }
    else if (EditButtonTakeSemaphore())
    {
        entriesToEdit--;
        if (entriesToEdit == 0)
        {
            SetTime12(timeSetting.hour.value, timeSetting.minute.value, timeSetting.PM_notAM.value);
            displayState = DISPLAYING;
            DisplayCurrentState();
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
    if (UpButtonTakeSemaphore())
    {
        bool increase = true;
        contrastSetting.adjust(increase);
        DisplayContrast();
    }
    else if (DownButtonTakeSemaphore())
    {
        bool increase = false;
        contrastSetting.adjust(increase);
        DisplayContrast();
    }
    else if (EditButtonTakeSemaphore())
    {
        entriesToEdit--;
        if (entriesToEdit == 0)
        {
            SetContrast(contrastSetting.value);
            displayState = DISPLAYING;
            lcd.ClearRow(3);
            DisplayCurrentState();
        }
    }
}
void HMI::EditBackLight()
{
    if (UpButtonTakeSemaphore())
    {
        bool increase = true;
        backlightSetting.adjust(increase);
        DisplayBacklight();
    }
    else if (DownButtonTakeSemaphore())
    {
        bool increase = false;
        backlightSetting.adjust(increase);
        DisplayBacklight();
    }
    else if (EditButtonTakeSemaphore())
    {
        entriesToEdit--;
        if (entriesToEdit == 0)
        {
            int index = backlightSetting.value;
            uint8_t r = backLightValues[index].r;
            uint8_t g = backLightValues[index].g;
            uint8_t b = backLightValues[index].b;
            SetBackLight(r, g, b);
            displayState = DISPLAYING;
            lcd.ClearRow(3);
            DisplayCurrentState();
        }
    }
}

void HMI::EditAlarmTime()
{
    if (UpButtonTakeSemaphore())
    {
        if (entriesToEdit == 3)
        {
            alarmSetting.hour.adjust(true);
        }
        else if (entriesToEdit == 2)
        {
            alarmSetting.minute.adjust(true);
        }
        else if (entriesToEdit == 1)
        {
            alarmSetting.PM_notAM.adjust(true);
        }

        DisplayAlarmSetting();
    }
    else if (DownButtonTakeSemaphore())
    {
        if (entriesToEdit == 3)
        {
            alarmSetting.hour.adjust(false);
        }
        else if (entriesToEdit == 2)
        {
            alarmSetting.minute.adjust(false);
        }
        else if (entriesToEdit == 1)
        {
            alarmSetting.PM_notAM.adjust(false);
        }
        DisplayAlarmSetting();
    }
    else if (EditButtonTakeSemaphore())
    {
        entriesToEdit--;
        if (entriesToEdit == 0)
        {
            uint8_t hour = alarmSetting.hour.value;
            uint8_t minute = alarmSetting.minute.value;
            bool PM_notAM = alarmSetting.PM_notAM.value;
            SetAlarmTime12(hour, minute, PM_notAM);
            displayState = DISPLAYING;
            lcd.ClearRow(3);
            DisplayCurrentState();
        }
    }
}
