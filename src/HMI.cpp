#include "HMI.h"
#include <sstream>
#include <iomanip>
#include "esp_log.h"

LCDState &operator++(LCDState &state)
{
    return state = (state == LCDState::EDITING) ? LCDState::DISPLAYING : static_cast<LCDState>(static_cast<int>(state) + 1);
}

LCDSettings &operator++(LCDSettings &state)
{
    return state = (state == LCDSettings::SETTING_TIME) ? LCDSettings::SETTING_DATE : static_cast<LCDSettings>(static_cast<int>(state) + 1);
}

HMI::HMI()
    : editButton(editPin),
      settingModeButton(settingModePin),
      downButton(downPin),
      upButton(upPin)
{
    lcd.Begin();
    lcd.ResetCursor();
    lcd.DisableSystemMessages();
    lcd.Display();
    lcd.SetBackLightFast(125, 125, 125);
}
void HMI::editing()
{
}
void HMI::display()
{
}
void HMI::process()
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
        default:
            break;
        }
    }

    if (editButton)
    {
        ++displayState;
        ESP_LOGI("BTN", "Display Mode State: %d", displayState);
    }
    else if (settingModeButton)
    {
        ++settingState;
        ESP_LOGI("BTN", "Setting Mode State: %d", settingState);
    }

    switch (displayState)
    {
    case DISPLAYING:
        editing();
        break;
    case EDITING:
        process();
        break;
    default:
        break;
    }
}
void HMI::updateDisplayTemperature(float temperatureF, float temperatureC)
{
    this->temperatureF = temperatureF;
    this->temperatureC = temperatureC;

    std::stringstream ss;
    if (displayTempF_notC)
    {
        ss << std::setprecision(5) << temperatureF << DEGREE_SYMBOL << "F";
    }
    else
    {
        ss << std::setprecision(5) << temperatureC << DEGREE_SYMBOL << "C";
    }

    std::string logtemp = ss.str();
    lcd.SetCursor(2, 0);
    lcd.WriteCharacters(logtemp.c_str(), logtemp.length());
}
void HMI::updateDisplayDateTime(DATE_TIME &dateTime)
{
    this->dateTime = dateTime;

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
}