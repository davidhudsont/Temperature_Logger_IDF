#include "DeviceCommands.h"

static CommandQueue tmp_command_queue;
static CommandQueue lcd_command_queue;
static CommandQueue date_command_queue;
static CommandQueue time_command_queue;
static CommandQueue alarm_command_queue;

static CommandSemaphore up_button_semaphore;
static CommandSemaphore down_button_semaphore;
static CommandSemaphore edit_button_semaphore;
static CommandSemaphore alt_button_semaphore;

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

void SetTime12(uint8_t hour, uint8_t minute, bool AMOrPM)
{
    time_command_queue.Send(SET_TIME12, hour, minute, AMOrPM);
}

bool RecieveTimeCommand(COMMAND_MESSAGE *msg)
{
    return time_command_queue.Recieve(msg);
}

void ReadDateTime()
{
    time_command_queue.Send(GET_DATETIME);
}

void DumpRTCRegisters()
{
    time_command_queue.Send(DUMP_RTC_REG);
}

// Button Tasks
void UpButtonGiveSemaphore()
{
    up_button_semaphore.GiveSemaphore();
}
void DownButtonGiveSemaphore()
{
    down_button_semaphore.GiveSemaphore();
}
void EditButtonGiveSemaphore()
{
    edit_button_semaphore.GiveSemaphore();
}
void AltButtonGiveSemaphore()
{
    alt_button_semaphore.GiveSemaphore();
}

bool UpButtonTakeSemaphore()
{
    return up_button_semaphore.TakeSemaphore();
}
bool DownButtonTakeSemaphore()
{
    return down_button_semaphore.TakeSemaphore();
}
bool EditButtonTakeSemaphore()
{
    return edit_button_semaphore.TakeSemaphore();
}
bool AltButtonTakeSemaphore()
{
    return alt_button_semaphore.TakeSemaphore();
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

void SetAlarmTime(uint8_t hour, uint8_t minute)
{
    // Adjust the time on the RTC.
    time_command_queue.Send(ALARM_TIME, hour, minute);
}

bool RecieveAlarmCommand(COMMAND_MESSAGE *msg)
{
    return alarm_command_queue.Recieve(msg);
}