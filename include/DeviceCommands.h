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
void readTemperature(bool FahrenheitOrCelsius);
bool recieveTMPCommand(COMMAND_MESSAGE *msg);

// LCD commands
void displayOff();
void displayOn();
void setContrast(uint8_t contrast);
void setBackLight(uint8_t r, uint8_t g, uint8_t b);
void clearDisplay();
void updateDisplay();
void resetDisplay();

bool recieveLCDCommand(COMMAND_MESSAGE *msg);

// Date Commands
void setWeekDay(uint8_t weekday);
void setMonth(uint8_t month);
void setYear(uint8_t year);
void setDayOfMonth(uint8_t dayOfMonth);
void setDate(uint8_t dayOfMonth, uint8_t month, uint8_t year);

bool recieveDateCommand(COMMAND_MESSAGE *msg);

// Time commands
void setSeconds(uint8_t second);
void setMinutes(uint8_t minute);
void setHours12Mode(uint8_t hour, bool AMOrPM);
void setHours24Mode(uint8_t hour);
void setTime(uint8_t hour, uint8_t minute, uint8_t second);

bool recieveTimeCommand(COMMAND_MESSAGE *msg);

void readDateTime();

// Button Tasks
void buttonPressed(BTN_COMMANDS command);

bool recieveButtonCommand(COMMAND_MESSAGE *msg);

// Alarm Commands
void setAlarm(bool on_off);
void setFrequency(uint32_t freq_hz);
void setDutyCycle(uint32_t duty_cycle);

bool recieveAlarmCommand(COMMAND_MESSAGE *msg);
