#include "DeviceCommands.h"
#include "freertos/queue.h"

static CommandQueue tmp_command_queue;
static CommandQueue lcd_command_queue;
static CommandQueue date_command_queue;
static CommandQueue time_command_queue;

void readTemperature(bool FahrenheitOrCelsius)
{
    COMMAND_MESSAGE msg;
    msg.id = FahrenheitOrCelsius ? GET_TEMPF : GET_TEMPC;
    tmp_command_queue.Send(msg);
}

bool recieveTMPCommand(COMMAND_MESSAGE *msg)
{
    return tmp_command_queue.Recieve(msg);
}

void displayOff()
{
    COMMAND_MESSAGE msg;
    msg.id = LCD_DISPLAY_OFF;
    lcd_command_queue.Send(msg);
}

void displayOn()
{
    COMMAND_MESSAGE msg;
    msg.id = LCD_DISPLAY_ON;
    lcd_command_queue.Send(msg);
}

void setContrast(uint8_t contrast)
{
    COMMAND_MESSAGE msg;
    msg.id = LCD_SET_CONTRAST;
    msg.arg1 = contrast;
    lcd_command_queue.Send(msg);
}

void SetBackLight(uint8_t r, uint8_t g, uint8_t b)
{
    COMMAND_MESSAGE msg;
    msg.id = LCD_SET_BACKLIGHT;
    msg.arg1 = r;
    msg.arg2 = g;
    msg.arg3 = b;
    lcd_command_queue.Send(msg);
}

void ClearDisplay()
{
    COMMAND_MESSAGE msg;
    msg.id = LCD_CLEAR_DISPLAY;
    lcd_command_queue.Send(msg);
}

bool recieveLCDCommand(COMMAND_MESSAGE *msg)
{
    return lcd_command_queue.Recieve(msg);
}

void setWeekDay(uint8_t weekday)
{
    COMMAND_MESSAGE msg;
    msg.id = SET_WEEKDAY;
    msg.arg1 = weekday;
    date_command_queue.Send(msg);
}

void setMonth(uint8_t month)
{
    COMMAND_MESSAGE msg;
    msg.id = SET_MONTH;
    msg.arg1 = month;
    date_command_queue.Send(msg);
}

void setYear(uint8_t year)
{
    COMMAND_MESSAGE msg;
    msg.id = SET_YEAR;
    msg.arg1 = year;
    date_command_queue.Send(msg);
}

void setDayOfMonth(uint8_t dayOfMonth)
{
    COMMAND_MESSAGE msg;
    msg.id = SET_DAYOFMONTH;
    msg.arg1 = dayOfMonth;
    date_command_queue.Send(msg);
}

bool recieveDateCommand(COMMAND_MESSAGE *msg)
{
    return date_command_queue.Recieve(msg);
}

void setSeconds(uint8_t second)
{
    COMMAND_MESSAGE msg;
    msg.id = SET_SECONDS;
    msg.arg1 = second;
    time_command_queue.Send(msg);
}

void setMinutes(uint8_t minute)
{
    COMMAND_MESSAGE msg;
    msg.id = SET_MINUTES;
    msg.arg1 = minute;
    time_command_queue.Send(msg);
}

void setHours12Mode(uint8_t hour, bool AMOrPM)
{
    COMMAND_MESSAGE msg;
    msg.id = SET_12HOURS;
    msg.arg1 = hour;
    msg.arg2 = AMOrPM;
    time_command_queue.Send(msg);
}

void setHours24Mode(uint8_t hour)
{
    COMMAND_MESSAGE msg;
    msg.id = SET_24HOURS;
    msg.arg1 = hour;
    time_command_queue.Send(msg);
}

bool recieveTimeCommand(COMMAND_MESSAGE *msg)
{
    return time_command_queue.Recieve(msg);
}

void readDateTime()
{
    COMMAND_MESSAGE msg;
    msg.id = GET_DATETIME;
    time_command_queue.Send(msg);
}
