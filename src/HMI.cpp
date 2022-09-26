
// RTOS
#include "esp_log.h"

// User Headers
#include "HMI.h"
#include "DeviceCommands.h"

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

static std::vector<Settings *> settingsList;

// Public
HMI::HMI()
    : settingMode("settings", 0, 0, 0)
{
    lcd.ResetCursor();
    lcd.DisableSystemMessages();
    lcd.Display();
    lcd.SetContrast(0);
    lcd.SetBackLightFast(125, 125, 125);

    settingsList.push_back(&dateSetting);
    settingsList.push_back(&timeSetting);
    settingsList.push_back(&tempSetting);
    settingsList.push_back(&contrastSetting);
    settingsList.push_back(&backlightSettings);
    settingsList.push_back(&alarmSetting);
    settingsList.push_back(&alarmEnableSetting);
    settingMode.set_max(settingsList.size() - 1);
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
        settingMode.increment();
        lcd.ClearRow(altSetnRow);
        DisplayCurrentState();
        DisplaySetting();
    }
    else if (DownButtonTakeSemaphore())
    {
        settingMode.decrement();
        lcd.ClearRow(altSetnRow);
        DisplayCurrentState();
        DisplaySetting();
    }
}

void HMI::DisplaySetting()
{
    ESP_LOGI("BTN", "Display Mode State: %d", displayState);
    lcd.SetCursor(altSetnRow, altSetnCol);
    int index = settingMode.get();
    std::string str = settingsList[index]->displayString();
    lcd.WriteString(str);
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
    static const size_t TempStringSize = 15;
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
    int index = settingMode.get();
    std::string str = settingsList[index]->getName();
    lcd.WriteString(str);
}

void HMI::UpdateDisplay()
{
    // lcd.Clear();
    DisplayTime();
    DisplayDate();
    DisplayTemperature();
    DisplayCurrentState();
}

void HMI::EditMode()
{
    int index = settingMode.get();
    if (UpButtonTakeSemaphore())
    {
        const Input input = Input::UP;
        settingsList[index]->getInput(input);
        lcd.SetCursor(altSetnRow, altSetnCol);
        std::string str = settingsList[index]->displayString();
        lcd.WriteString(str);
        DisplayCurrentState();
    }
    else if (DownButtonTakeSemaphore())
    {
        const Input input = Input::DOWN;
        settingsList[index]->getInput(input);
        lcd.SetCursor(altSetnRow, altSetnCol);
        std::string str = settingsList[index]->displayString();
        lcd.WriteString(str);
        DisplayCurrentState();
    }
    else if (EditButtonTakeSemaphore())
    {
        const Input input = Input::ENTER;
        if (settingsList[index]->getInput(input))
        {
            displayState = DISPLAYING;
            DisplayCurrentState();
        }
    }
}