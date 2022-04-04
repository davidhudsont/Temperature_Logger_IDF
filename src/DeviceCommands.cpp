#include "DeviceCommands.h"

static CommandQueue tmp_command_queue;
static CommandQueue lcd_command_queue;
static CommandQueue date_command_queue;
static CommandQueue time_command_queue;
static CommandQueue button_command_queue;
static CommandQueue alarm_command_queue;

// Temperature Commands
void readTemperature(bool FahrenheitOrCelsius)
{
    TMP_COMMANDS id = FahrenheitOrCelsius ? GET_TEMPF : GET_TEMPC;
    tmp_command_queue.Send(id);
}

bool recieveTMPCommand(COMMAND_MESSAGE *msg)
{
    return tmp_command_queue.Recieve(msg);
}

// LCD Commands
void displayOff()
{
    lcd_command_queue.Send(LCD_DISPLAY_OFF);
}

void displayOn()
{
    lcd_command_queue.Send(LCD_DISPLAY_ON);
}

void setContrast(uint8_t contrast)
{
    lcd_command_queue.Send(LCD_SET_CONTRAST, contrast);
}

void setBackLight(uint8_t r, uint8_t g, uint8_t b)
{
    lcd_command_queue.Send(LCD_SET_BACKLIGHT, r, g, b);
}

void clearDisplay()
{
    lcd_command_queue.Send(LCD_CLEAR_DISPLAY);
}

void updateDisplay()
{
    lcd_command_queue.Send(LCD_DISPLAY_UPDATE);
}

void resetDisplay()
{
    lcd_command_queue.Send(LCD_RESET);
}

bool recieveLCDCommand(COMMAND_MESSAGE *msg)
{
    return lcd_command_queue.Recieve(msg);
}

// Date Commands
void setWeekDay(uint8_t weekday)
{
    date_command_queue.Send(SET_WEEKDAY, weekday);
}

void setMonth(uint8_t month)
{
    date_command_queue.Send(SET_MONTH, month);
}

void setYear(uint8_t year)
{
    date_command_queue.Send(SET_YEAR, year);
}

void setDayOfMonth(uint8_t dayOfMonth)
{
    date_command_queue.Send(SET_DAYOFMONTH, dayOfMonth);
}

void setDate(uint8_t dayOfMonth, uint8_t month, uint8_t year)
{
    date_command_queue.Send(SET_DATE, dayOfMonth, month, year);
}

bool recieveDateCommand(COMMAND_MESSAGE *msg)
{
    return date_command_queue.Recieve(msg);
}

// Time Commands
void setSeconds(uint8_t second)
{
    time_command_queue.Send(SET_SECONDS, second);
}

void setMinutes(uint8_t minute)
{
    time_command_queue.Send(SET_MINUTES, minute);
}

void setHours12Mode(uint8_t hour, bool AMOrPM)
{
    time_command_queue.Send(SET_12HOURS, hour, AMOrPM);
}

void setHours24Mode(uint8_t hour)
{
    time_command_queue.Send(SET_24HOURS, hour);
}

void setTime(uint8_t hour, uint8_t minute, uint8_t second)
{
    time_command_queue.Send(SET_TIME, hour, minute, second);
}

bool recieveTimeCommand(COMMAND_MESSAGE *msg)
{
    return time_command_queue.Recieve(msg);
}

void readDateTime()
{
    time_command_queue.Send(GET_DATETIME);
}

// Button Tasks
void buttonPressed(BTN_COMMANDS command)
{
    button_command_queue.Send(command);
}

bool recieveButtonCommand(COMMAND_MESSAGE *msg)
{
    return button_command_queue.Recieve(msg);
}

// Alarm Commands
void setAlarm(bool on_off)
{
    alarm_command_queue.Send(ALARM_SET, on_off);
}

void setFrequency(uint32_t freq_hz)
{
    alarm_command_queue.Send(ALARM_FREQ, freq_hz);
}

void setDutyCycle(uint32_t duty_cycle)
{
    alarm_command_queue.Send(ALARM_DUTY_CYCLE, duty_cycle);
}

bool recieveAlarmCommand(COMMAND_MESSAGE *msg)
{
    return alarm_command_queue.Recieve(msg);
}