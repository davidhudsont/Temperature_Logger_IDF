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
    SET_DATE,
    SET_MONTH,
    SET_YEAR,
    GET_DATETIME,
    GET_TEMP,
    LCD_DISPLAY_OFF,
    LCD_DISPLAY_ON,
    LCD_SET_CONTRAST,
    LCD_SET_BACKLIGHT,
    LCD_CLEAR_DISPLAY,
};

void readTemperature();
bool recieveTMPCommand(COMMAND_MESSAGE *msg);

void displayOff();
void displayOn();
void setContrast(uint8_t contrast);
void SetBackLightFast(uint8_t r, uint8_t g, uint8_t b);

bool recieveLCDCommand(COMMAND_MESSAGE *msg);

void setWeekDay(uint8_t weekday);
void setMonth(uint8_t month);
void setYear(uint8_t year);
void setDate(uint8_t dayOfMonth, uint8_t month, uint8_t year);

bool recieveDateCommand(COMMAND_MESSAGE *msg);

void setSeconds(uint8_t second);
void setMinutes(uint8_t minute);
void setHours12Mode(uint8_t hour, uint8_t AMOrPM);
void setHours24Mode(uint8_t hour);

bool recieveTimeCommand(COMMAND_MESSAGE *msg);
