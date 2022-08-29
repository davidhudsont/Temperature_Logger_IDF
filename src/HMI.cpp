
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
static const size_t contrastStringSize = 14;
static const size_t TempStringSize = 15;

static char backlightString[backlightStringSize];
static char contrastString[contrastStringSize];

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

    settingMode.max_value = (int)SETTINGS_COUNT - 1;
    settingMode.min_value = 0;

    contrastSetting.max_value = 255;
    contrastSetting.min_value = 0;
    contrastSetting.value = 0;

    backlightSettings.max_value = (int)COLOR_COUNT - 1;
    backlightSettings.min_value = 0;
    backlightSettings.value = 0;

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
    dateSetting.setSetting("month", dateTime.month);
    dateSetting.setSetting("dayOfMonth", dateTime.dayofMonth);
    dateSetting.setSetting("year", dateTime.year);

    timeSetting.setSetting("hour", dateTime.hour);
    timeSetting.setSetting("minute", dateTime.minute);
    timeSetting.setSetting("amPm", dateTime.PM_notAM);
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
        DisplaySetting();
    }
    else if (AltButtonTakeSemaphore())
    {
        SetAlarm(false);
    }
    else if (UpButtonTakeSemaphore())
    {
        settingMode.adjust(true);
        lcd.ClearRow(altSetnRow);
        DisplaySetting();
    }
    else if (DownButtonTakeSemaphore())
    {
        settingMode.adjust(false);
        lcd.ClearRow(altSetnRow);
        DisplaySetting();
    }
}

void HMI::DisplaySetting()
{
    ESP_LOGI("BTN", "Display Mode State: %d", displayState);
    switch (settingMode.value)
    {
    case SETTING_DATE:
        entriesToEdit = 3;
        DisplayCurrentState();
        lcd.ClearRow(altSetnRow);
        break;
    case SETTING_TIME:
        entriesToEdit = 3;
        DisplayCurrentState();
        lcd.ClearRow(altSetnRow);
        break;
    case SETTING_TEMP:
        entriesToEdit = 1;
        DisplayCurrentState();
        lcd.ClearRow(altSetnRow);
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

void HMI::DisplayDate()
{
    // Date Update
    lcd.SetCursor(dateRow, dateCol);
    std::string str = dateSetting.displayString();
    lcd.WriteString(str);
}

void HMI::DisplayTime()
{
    // Time update
    lcd.SetCursor(timeRow, timeCol);
    std::string str = timeSetting.displayString();
    lcd.WriteString(str);
}

void HMI::DisplayTemperature()
{
    static char tempString[TempStringSize];
    lcd.SetCursor(tempRow, tempCol);
    std::string units = tempSetting.displayString();
    if (tempSetting.getSetting("units"))
    {
        if (temperatureF > 100)
            snprintf(tempString, TempStringSize, "%3.2f", temperatureF);
        else
            snprintf(tempString, TempStringSize, "%2.3f", temperatureF);
        std::string temperature = std::string(tempString, 5);
        std::string result = temperature + units;
        lcd.WriteString(result);
    }
    else
    {
        snprintf(tempString, TempStringSize, "%2.3f", temperatureC);
        std::string temperature = std::string(tempString, 5);
        std::string result = temperature + units;
        lcd.WriteString(result);
    }
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
    // lcd.Clear();
    DisplayTime();
    DisplayDate();
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
    snprintf(backlightString, backlightStringSize, "Backlight: %s", LCD_BCKL_COLORS[backlightSettings.value]);
    lcd.WriteCharacters(backlightString, backlightStringSize - 1);
}

void HMI::DisplayAlarmSetting()
{
    lcd.SetCursor(altSetnRow, altSetnCol);
    std::string str = alarmSetting.displayString();
    lcd.WriteString(str);
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
        const Input input = Input::UP;
        dateSetting.getInput(input);
        DisplayDate();
    }
    else if (DownButtonTakeSemaphore())
    {
        const Input input = Input::DOWN;
        dateSetting.getInput(input);
        DisplayDate();
    }
    else if (EditButtonTakeSemaphore())
    {
        const Input input = Input::ENTER;
        if (dateSetting.getInput(input))
        {
            displayState = DISPLAYING;
            DisplayCurrentState();
        }
    }
}

void HMI::EditingTime()
{
    if (UpButtonTakeSemaphore())
    {
        const Input input = Input::UP;
        timeSetting.getInput(input);
        DisplayTime();
    }
    else if (DownButtonTakeSemaphore())
    {
        const Input input = Input::DOWN;
        timeSetting.getInput(input);
        DisplayTime();
    }
    else if (EditButtonTakeSemaphore())
    {
        const Input input = Input::ENTER;
        if (timeSetting.getInput(input))
        {
            displayState = DISPLAYING;
            DisplayCurrentState();
        }
    }
}

void HMI::ChangeTemp()
{
    Input input = Input::ENTER;
    tempSetting.getInput(input);
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
        backlightSettings.adjust(increase);
        DisplayBacklight();
    }
    else if (DownButtonTakeSemaphore())
    {
        bool increase = false;
        backlightSettings.adjust(increase);
        DisplayBacklight();
    }
    else if (EditButtonTakeSemaphore())
    {
        entriesToEdit--;
        if (entriesToEdit == 0)
        {
            int index = backlightSettings.value;
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
        Input input = Input::UP;
        alarmSetting.getInput(input);
        DisplayAlarmSetting();
    }
    else if (DownButtonTakeSemaphore())
    {
        Input input = Input::DOWN;
        alarmSetting.getInput(input);
        DisplayAlarmSetting();
    }
    else if (EditButtonTakeSemaphore())
    {
        Input input = Input::ENTER;
        if (alarmSetting.getInput(input))
        {
            displayState = DISPLAYING;
            lcd.ClearRow(3);
            DisplayCurrentState();
        }
    }
}

void HMI::DisplayAlarmEnable()
{
    lcd.SetCursor(3, 0);
    std::string str = alarmEnableSetting.displayString();
    lcd.WriteString(str);
}

void HMI::EditAlarmEnable()
{
    Input input = Input::ENTER;
    alarmEnableSetting.getInput(input);
    displayState = DISPLAYING;
    DisplayAlarmEnable();
    DisplayCurrentState();
}
