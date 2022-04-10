#pragma once

#include "stdint.h"
#include "CommandQueue.h"

enum RTC_COMMANDS
{
    SET_SECONDS = 0,
    SET_MINUTES,
    SET_12HOURS,
    SET_24HOURS,
    SET_TIME,
    SET_WEEKDAY,
    SET_DAYOFMONTH,
    SET_MONTH,
    SET_YEAR,
    SET_DATE,
    GET_DATETIME,
    ALARM_TIME,
};

enum TMP_COMMANDS
{
    GET_TEMPF,
    GET_TEMPC,
};

enum LCD_COMMANDS
{
    LCD_DISPLAY_OFF,
    LCD_DISPLAY_ON,
    LCD_SET_CONTRAST,
    LCD_SET_BACKLIGHT,
    LCD_CLEAR_DISPLAY,
    LCD_DISPLAY_UPDATE,
    LCD_RESET,
};

enum BTN_COMMANDS
{
    ALT_BTN_PRESSED,
    EDIT_MODE_PRESSED,
    DOWN_PRESSED,
    UP_PRESSED,
};

enum ALARM_COMMANDS
{
    ALARM_SET,
    ALARM_FREQ,
    ALARM_DUTY_CYCLE,
};

// Temperature Commands
void ReadTemperature(bool FahrenheitOrCelsius);
bool RecieveTMPCommand(COMMAND_MESSAGE *msg);

// LCD commands
void DisplayOff();
void DisplayOn();
void SetContrast(uint8_t contrast);
void SetBackLight(uint8_t r, uint8_t g, uint8_t b);
void ClearDisplay();
void UpdateDisplay();
void ResetDisplay();

bool RecieveLCDCommand(COMMAND_MESSAGE *msg);

// Date Commands
void SetWeekDay(uint8_t weekday);
void SetMonth(uint8_t month);
void SetYear(uint8_t year);
void SetDayOfMonth(uint8_t dayOfMonth);
void SetDate(uint8_t dayOfMonth, uint8_t month, uint8_t year);

bool RecieveDateCommand(COMMAND_MESSAGE *msg);

// Time commands
void SetSeconds(uint8_t second);
void SetMinutes(uint8_t minute);
void SetHours12Mode(uint8_t hour, bool AMOrPM);
void SetHours24Mode(uint8_t hour);
void SetTime(uint8_t hour, uint8_t minute, uint8_t second);

bool RecieveTimeCommand(COMMAND_MESSAGE *msg);

void ReadDateTime();

// Button Tasks
void ButtonPressed(BTN_COMMANDS command);

bool RecieveButtonCommand(COMMAND_MESSAGE *msg);

// Alarm Commands
void SetAlarm(bool on_off);
void SetFrequency(uint32_t freq_hz);
void SetDutyCycle(uint32_t duty_cycle);
void SetAlarmTime(uint8_t hour, uint8_t minute);

bool RecieveAlarmCommand(COMMAND_MESSAGE *msg);
