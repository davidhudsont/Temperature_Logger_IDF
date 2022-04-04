#include "DeviceCommands.h"

static CommandQueue tmp_command_queue;
static CommandQueue lcd_command_queue;
static CommandQueue date_command_queue;
static CommandQueue time_command_queue;
static CommandQueue button_command_queue;
static CommandQueue alarm_command_queue;

// Temperature Commands
void ReadTemperature(bool FahrenheitOrCelsius)
{
    TMP_COMMANDS id = FahrenheitOrCelsius ? GET_TEMPF : GET_TEMPC;
    tmp_command_queue.Send(id);
}

bool RecieveTMPCommand(COMMAND_MESSAGE *msg)
{
    return tmp_command_queue.Recieve(msg);
}

// LCD Commands
void DisplayOff()
{
    lcd_command_queue.Send(LCD_DISPLAY_OFF);
}

void DisplayOn()
{
    lcd_command_queue.Send(LCD_DISPLAY_ON);
}

void SetContrast(uint8_t contrast)
{
    lcd_command_queue.Send(LCD_SET_CONTRAST, contrast);
}

void SetBackLight(uint8_t r, uint8_t g, uint8_t b)
{
    lcd_command_queue.Send(LCD_SET_BACKLIGHT, r, g, b);
}

void ClearDisplay()
{
    lcd_command_queue.Send(LCD_CLEAR_DISPLAY);
}

void UpdateDisplay()
{
    lcd_command_queue.Send(LCD_DISPLAY_UPDATE);
}

void ResetDisplay()
{
    lcd_command_queue.Send(LCD_RESET);
}

bool RecieveLCDCommand(COMMAND_MESSAGE *msg)
{
    return lcd_command_queue.Recieve(msg);
}

// Date Commands
void SetWeekDay(uint8_t weekday)
{
    date_command_queue.Send(SET_WEEKDAY, weekday);
}

void SetMonth(uint8_t month)
{
    date_command_queue.Send(SET_MONTH, month);
}

void SetYear(uint8_t year)
{
    date_command_queue.Send(SET_YEAR, year);
}

void SetDayOfMonth(uint8_t dayOfMonth)
{
    date_command_queue.Send(SET_DAYOFMONTH, dayOfMonth);
}

void SetDate(uint8_t dayOfMonth, uint8_t month, uint8_t year)
{
    date_command_queue.Send(SET_DATE, dayOfMonth, month, year);
}

bool RecieveDateCommand(COMMAND_MESSAGE *msg)
{
    return date_command_queue.Recieve(msg);
}

// Time Commands
void SetSeconds(uint8_t second)
{
    time_command_queue.Send(SET_SECONDS, second);
}

void SetMinutes(uint8_t minute)
{
    time_command_queue.Send(SET_MINUTES, minute);
}

void SetHours12Mode(uint8_t hour, bool AMOrPM)
{
    time_command_queue.Send(SET_12HOURS, hour, AMOrPM);
}

void SetHours24Mode(uint8_t hour)
{
    time_command_queue.Send(SET_24HOURS, hour);
}

void SetTime(uint8_t hour, uint8_t minute, uint8_t second)
{
    time_command_queue.Send(SET_TIME, hour, minute, second);
}

bool RecieveTimeCommand(COMMAND_MESSAGE *msg)
{
    return time_command_queue.Recieve(msg);
}

void ReadDateTime()
{
    time_command_queue.Send(GET_DATETIME);
}

// Button Tasks
void ButtonPressed(BTN_COMMANDS command)
{
    button_command_queue.Send(command);
}

bool RecieveButtonCommand(COMMAND_MESSAGE *msg)
{
    return button_command_queue.Recieve(msg);
}

// Alarm Commands
void SetAlarm(bool on_off)
{
    alarm_command_queue.Send(ALARM_SET, on_off);
}

void SetFrequency(uint32_t freq_hz)
{
    alarm_command_queue.Send(ALARM_FREQ, freq_hz);
}

void SetDutyCycle(uint32_t duty_cycle)
{
    alarm_command_queue.Send(ALARM_DUTY_CYCLE, duty_cycle);
}

bool RecieveAlarmCommand(COMMAND_MESSAGE *msg)
{
    return alarm_command_queue.Recieve(msg);
}