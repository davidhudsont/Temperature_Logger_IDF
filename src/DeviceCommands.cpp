#include "DeviceCommands.h"

static CommandQueue tmp_command_queue;
static CommandQueue lcd_command_queue;
static CommandQueue date_command_queue;
static CommandQueue time_command_queue;
static CommandQueue button_command_queue;
static CommandQueue alarm_command_queue;

static void sendCommand(CommandQueue &queue, int id, int arg1 = 0, int arg2 = 0, int arg3 = 0)
{
    COMMAND_MESSAGE msg;
    msg.id = id;
    msg.arg1 = 0;
    msg.arg2 = 0;
    msg.arg3 = 0;
    queue.Send(msg);
}

// Temperature Commands
void readTemperature(bool FahrenheitOrCelsius)
{
    TMP_COMMANDS id = FahrenheitOrCelsius ? GET_TEMPF : GET_TEMPC;
    sendCommand(tmp_command_queue, id);
}

bool recieveTMPCommand(COMMAND_MESSAGE *msg)
{
    return tmp_command_queue.Recieve(msg);
}

// LCD Commands
void displayOff()
{
    sendCommand(lcd_command_queue, LCD_DISPLAY_OFF);
}

void displayOn()
{
    sendCommand(lcd_command_queue, LCD_DISPLAY_ON);
}

void setContrast(uint8_t contrast)
{
    sendCommand(lcd_command_queue, LCD_SET_CONTRAST, contrast);
}

void setBackLight(uint8_t r, uint8_t g, uint8_t b)
{
    sendCommand(lcd_command_queue, LCD_SET_BACKLIGHT, r, g, b);
}

void clearDisplay()
{
    sendCommand(lcd_command_queue, LCD_CLEAR_DISPLAY);
}

void updateDisplay()
{
    sendCommand(lcd_command_queue, LCD_DISPLAY_UPDATE);
}

void resetDisplay()
{
    sendCommand(lcd_command_queue, LCD_RESET);
}

bool recieveLCDCommand(COMMAND_MESSAGE *msg)
{
    return lcd_command_queue.Recieve(msg);
}

// Date Commands
void setWeekDay(uint8_t weekday)
{
    sendCommand(date_command_queue, SET_WEEKDAY, weekday);
}

void setMonth(uint8_t month)
{
    sendCommand(date_command_queue, SET_MONTH, month);
}

void setYear(uint8_t year)
{
    sendCommand(date_command_queue, SET_YEAR, year);
}

void setDayOfMonth(uint8_t dayOfMonth)
{
    sendCommand(date_command_queue, SET_DAYOFMONTH, dayOfMonth);
}

void setDate(uint8_t dayOfMonth, uint8_t month, uint8_t year)
{
    sendCommand(date_command_queue, SET_DATE, dayOfMonth, month, year);
}

bool recieveDateCommand(COMMAND_MESSAGE *msg)
{
    return date_command_queue.Recieve(msg);
}

// Time Commands
void setSeconds(uint8_t second)
{
    sendCommand(time_command_queue, SET_SECONDS, second);
}

void setMinutes(uint8_t minute)
{
    sendCommand(time_command_queue, SET_MINUTES, minute);
}

void setHours12Mode(uint8_t hour, bool AMOrPM)
{
    sendCommand(time_command_queue, SET_12HOURS, hour, AMOrPM);
}

void setHours24Mode(uint8_t hour)
{
    sendCommand(time_command_queue, SET_24HOURS, hour);
}

void setTime(uint8_t hour, uint8_t minute, uint8_t second)
{
    sendCommand(time_command_queue, SET_TIME, hour, minute, second);
}

bool recieveTimeCommand(COMMAND_MESSAGE *msg)
{
    return time_command_queue.Recieve(msg);
}

void readDateTime()
{
    sendCommand(time_command_queue, GET_DATETIME);
}

// Button Tasks
void buttonPressed(BTN_COMMANDS command)
{
    sendCommand(button_command_queue, command);
}

bool recieveButtonCommand(COMMAND_MESSAGE *msg)
{
    return button_command_queue.Recieve(msg);
}

// Alarm Commands
void setAlarm(bool on_off)
{
    sendCommand(alarm_command_queue, ALARM_SET, on_off);
}

void setFrequency(uint32_t freq_hz)
{
    sendCommand(alarm_command_queue, ALARM_FREQ, freq_hz);
}

void setDutyCycle(uint32_t duty_cycle)
{
    sendCommand(alarm_command_queue, ALARM_DUTY_CYCLE, duty_cycle);
}

bool recieveAlarmCommand(COMMAND_MESSAGE *msg)
{
    return alarm_command_queue.Recieve(msg);
}