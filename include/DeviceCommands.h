#pragma once

#include "stdint.h"
#include "CommandQueue.h"

enum COMMANDS
{
    EMPTY = -1,
    SET_SECONDS = 0,
    SET_MINUTES,
    SET_12HOURS,
    SET_24HOURS,
    SET_WEEKDAY,
    SET_DAYOFMONTH,
    SET_MONTH,
    SET_YEAR,
    GET_DATETIME,
    GET_TEMPF,
    GET_TEMPC,
    LCD_DISPLAY_OFF,
    LCD_DISPLAY_ON,
    LCD_SET_CONTRAST,
    LCD_SET_BACKLIGHT,
    LCD_CLEAR_DISPLAY,
};

void readTemperature(bool FahrenheitOrCelsius);
bool recieveTMPCommand(COMMAND_MESSAGE *msg);

void displayOff();
void displayOn();
void setContrast(uint8_t contrast);
void SetBackLight(uint8_t r, uint8_t g, uint8_t b);
void ClearDisplay();

bool recieveLCDCommand(COMMAND_MESSAGE *msg);

void setWeekDay(uint8_t weekday);
void setMonth(uint8_t month);
void setYear(uint8_t year);
void setDayOfMonth(uint8_t dayOfMonth);

bool recieveDateCommand(COMMAND_MESSAGE *msg);

void setSeconds(uint8_t second);
void setMinutes(uint8_t minute);
void setHours12Mode(uint8_t hour, bool AMOrPM);
void setHours24Mode(uint8_t hour);

bool recieveTimeCommand(COMMAND_MESSAGE *msg);

void readDateTime();