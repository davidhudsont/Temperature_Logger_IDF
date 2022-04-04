#include "ConsoleCommands.h"
#include "esp_log.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "DeviceCommands.h"

static void RegisterTimeCommand(void);
static void RegisterDateCommand(void);
static void RegisterGetdatetimeCommand(void);
static void RegisterTemperatureCommand(void);
static void RegisterLogLevelCommand(void);
static void RegisterLcdCommand(void);
static void RegisterAlarmCommand(void);

// cppcheck-suppress unusedFunction
void RegisterConsoleCommands(void)
{
    RegisterTimeCommand();
    RegisterDateCommand();
    RegisterGetdatetimeCommand();
    RegisterTemperatureCommand();
    RegisterLogLevelCommand();
    RegisterLcdCommand();
    RegisterAlarmCommand();
}

static struct
{
    struct arg_int *seconds;
    struct arg_int *minutes;
    struct arg_int *hours12;
    struct arg_int *hours24;
    struct arg_end *end;
} time_args;

static int set_time(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&time_args);

    if (nerrors != 0)
    {
        arg_print_errors(stderr, time_args.end, argv[0]);
        return 1;
    }

    if (time_args.seconds->count)
    {
        uint8_t seconds = time_args.seconds->ival[0];
        ESP_LOGI("RTC", "Set seconds to: %d", time_args.seconds->ival[0]);
        setSeconds(seconds);
    }
    else if (time_args.minutes->count)
    {
        uint8_t minutes = time_args.minutes->ival[0];
        ESP_LOGI("RTC", "Set minutes to: %d", minutes);
        setMinutes(minutes);
    }
    else if (time_args.hours12->count)
    {
        bool PM_notAM = time_args.hours12->ival[1];
        uint8_t hours = time_args.hours12->ival[0];
        ESP_LOGI("RTC", "Set hours to: %d, %s", hours, (PM_notAM ? "PM" : "AM"));
        setHours12Mode(hours, PM_notAM);
    }
    else if (time_args.hours24->count)
    {
        uint8_t hours = time_args.hours24->ival[0];
        ESP_LOGI("RTC", "Set hours to: %d", hours);
        setHours24Mode(hours);
    }

    return 0;
}

static void RegisterTimeCommand(void)
{
    time_args.seconds = arg_int0("s", "seconds", "<0-59>", "Set seconds!");
    time_args.minutes = arg_int0("m", "minutes", "<0-59>", "Set minutes!");
    time_args.hours12 = arg_intn("h12", "hours12", "<1-12>, <0 AM|1 PM>", 0, 2, "Set hours in 12 hour format!");
    time_args.hours24 = arg_int0("h24", "hours24", "<0-23>", "Set hours in 24 hour format!");
    time_args.end = arg_end(4);

    const esp_console_cmd_t cmd = {
        .command = "time",
        .help = "Time Commands",
        .hint = NULL,
        .func = &set_time,
        .argtable = &time_args,
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static struct
{
    struct arg_int *days;
    struct arg_int *date;
    struct arg_int *month;
    struct arg_int *year;
    struct arg_end *end;
} date_args;

static int set_date(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&date_args);

    if (nerrors != 0)
    {
        arg_print_errors(stderr, date_args.end, argv[0]);
        return 1;
    }

    if (date_args.days->count)
    {
        int days = date_args.days->ival[0];
        ESP_LOGI("RTC", "Set Days to: %d", date_args.days->ival[0]);
        setWeekDay(days);
    }
    else if (date_args.date->count)
    {
        int dayOfMonth = date_args.date->ival[0];
        ESP_LOGI("RTC", "Set Date to: %d", dayOfMonth);
        setDayOfMonth(dayOfMonth);
    }
    else if (date_args.month->count)
    {
        int month = date_args.month->ival[0];
        ESP_LOGI("RTC", "Set Month to: %d", month);
        setMonth(month);
    }
    else if (date_args.year->count)
    {
        int year = date_args.year->ival[0];
        ESP_LOGI("RTC", "Set Year to: %d", year);
        setYear(year);
    }

    return 0;
}

static void RegisterDateCommand(void)
{
    date_args.days = arg_int0("w", NULL, "<1-7>", "Set weekday!");
    date_args.date = arg_int0("d", NULL, "<1-31>", "Set the day of the month!");
    date_args.month = arg_int0("m", NULL, "<1-12>", "Set the months!");
    date_args.year = arg_int0("y", NULL, "<0-99>", "Set the year!");
    date_args.end = arg_end(4);

    const esp_console_cmd_t cmd = {
        .command = "date",
        .help = "Date Commands",
        .hint = NULL,
        .func = &set_date,
        .argtable = &date_args,
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static int get_datetime(int argc, char **argv)
{
    readDateTime();
    return 0;
}

static void RegisterGetdatetimeCommand(void)
{
    const esp_console_cmd_t cmd = {
        .command = "datetime",
        .help = "Print the Date & Time",
        .hint = NULL,
        .func = &get_datetime,
        .argtable = NULL,
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static struct
{
    struct arg_lit *tempf;
    struct arg_lit *tempc;
    struct arg_end *end;
} temperature_args;

static int get_temperature(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&temperature_args);

    if (nerrors != 0)
    {
        arg_print_errors(stderr, temperature_args.end, argv[0]);
        return 1;
    }

    if (temperature_args.tempf->count)
    {
        readTemperature(true);
    }
    if (temperature_args.tempc->count)
    {
        readTemperature(false);
    }
    return 0;
}

static void RegisterTemperatureCommand(void)
{
    temperature_args.tempf = arg_lit0("f", NULL, "Get temperature in Fahrenheit!");
    temperature_args.tempc = arg_lit0("c", NULL, "Get temperature in Celsius!");
    temperature_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "tmp",
        .help = "Temperature Commands",
        .hint = NULL,
        .func = &get_temperature,
        .argtable = &temperature_args,
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static struct
{
    struct arg_int *level;
    struct arg_end *end;
} level_args;

static int set_log_level(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&level_args);

    if (nerrors != 0)
    {
        arg_print_errors(stderr, level_args.end, argv[0]);
        return 1;
    }

    if (level_args.level->count)
    {
        int loglevel = level_args.level->ival[0];
        if (loglevel >= ESP_LOG_NONE && loglevel <= ESP_LOG_VERBOSE)
        {
            esp_log_level_set("*", (esp_log_level_t)loglevel);
            ESP_LOGI("LOG", "Setting Log Level %d", loglevel);
        }
        else
        {
            ESP_LOGI("LOG", "Invalid Log Level %d", loglevel);
        }
    }
    return 0;
}

static void RegisterLogLevelCommand(void)
{
    level_args.level = arg_int0("l", NULL, "<0-5>", "Set the log level");
    level_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "log",
        .help = "Set log level",
        .hint = NULL,
        .func = &set_log_level,
        .argtable = &level_args,
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static struct
{
    struct arg_int *display_toggle;
    struct arg_int *contrast;
    struct arg_int *backlight;
    struct arg_lit *clear;
    struct arg_lit *reset;
    struct arg_end *end;
} lcd_args;

static int lcd(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&lcd_args);

    if (nerrors != 0)
    {
        arg_print_errors(stderr, lcd_args.end, argv[0]);
        return 1;
    }

    if (lcd_args.display_toggle->count)
    {
        bool turnDisplayOn = lcd_args.display_toggle->ival[0] ? true : false;
        if (turnDisplayOn)
        {
            ESP_LOGI("LCD", "Turn Display On");
            displayOn();
        }
        else
        {
            ESP_LOGI("LCD", "Turn Display Off");
            displayOff();
        }
    }
    else if (lcd_args.contrast->count)
    {
        uint8_t contrast = lcd_args.contrast->ival[0];
        ESP_LOGI("LCD", "Set Contrast to %d", contrast);
        setContrast(contrast);
    }
    else if (lcd_args.backlight->count)
    {
        if (lcd_args.backlight->count == 1)
        {
            uint8_t r = lcd_args.backlight->ival[0];
            uint8_t g = 0;
            uint8_t b = 0;
            setBackLight(r, g, b);
            ESP_LOGI("LCD", "Set backlight to %d, %d, %d", r, g, b);
        }
        else if (lcd_args.backlight->count == 2)
        {
            uint8_t r = lcd_args.backlight->ival[0];
            uint8_t g = lcd_args.backlight->ival[1];
            uint8_t b = 0;
            ESP_LOGI("LCD", "Set backlight to %d, %d, %d", r, g, b);
            setBackLight(r, g, b);
        }
        else if (lcd_args.backlight->count == 3)
        {
            uint8_t r = lcd_args.backlight->ival[0];
            uint8_t g = lcd_args.backlight->ival[1];
            uint8_t b = lcd_args.backlight->ival[2];
            ESP_LOGI("LCD", "Set backlight to %d, %d, %d", r, g, b);
            setBackLight(r, g, b);
        }
    }
    else if (lcd_args.clear->count)
    {
        ESP_LOGI("LCD", "Clear Display");
        clearDisplay();
    }
    else if (lcd_args.reset->count)
    {
        ESP_LOGI("LCD", "Reset Display");
        resetDisplay();
    }

    return 0;
}

static void RegisterLcdCommand(void)
{

    lcd_args.display_toggle = arg_int0("d", NULL, "<bool>", "Turn Display On/Off");
    lcd_args.contrast = arg_int0("l", NULL, "<0-255>", "Set the Contrast");
    lcd_args.backlight = arg_intn("b", NULL, "<0-255>r, <0-255>g, <0-255>b", 0, 3, "Set the backlight rgb");
    lcd_args.clear = arg_lit0("c", NULL, "Clear the Display");
    lcd_args.reset = arg_lit0("r", NULL, "Reset the Display");
    lcd_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "lcd",
        .help = "LCD Commands",
        .hint = NULL,
        .func = &lcd,
        .argtable = &lcd_args};

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static struct
{
    struct arg_int *alarm_speaker_control;
    struct arg_int *alarm_frequency;
    struct arg_int *alarm_duty_cycle;
    struct arg_end *end;
} alarm_speaker_args;

static int alarm_speaker(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&alarm_speaker_args);

    if (nerrors != 0)
    {
        arg_print_errors(stderr, alarm_speaker_args.end, argv[0]);
        return 1;
    }

    if (alarm_speaker_args.alarm_speaker_control->count)
    {
        bool turnAlarmOn = alarm_speaker_args.alarm_speaker_control->ival[0] ? true : false;
        if (turnAlarmOn)
        {
            ESP_LOGI("ALARM", "Turn Alarm On");
            setAlarm(true);
        }
        else
        {
            ESP_LOGI("ALARM", "Turn Alarm Off");
            setAlarm(false);
        }
    }
    if (alarm_speaker_args.alarm_frequency->count)
    {
        uint32_t freq_hz = alarm_speaker_args.alarm_frequency->ival[0];
        ESP_LOGI("ALARM", "Set Frequency to %dHz", freq_hz);
        setFrequency(freq_hz);
    }
    if (alarm_speaker_args.alarm_duty_cycle->count)
    {
        uint32_t duty_cycle = alarm_speaker_args.alarm_duty_cycle->ival[0];
        ESP_LOGI("ALARM", "Set Duty Cycle to %d", duty_cycle);
        setDutyCycle(duty_cycle);
    }

    return 0;
}

static void RegisterAlarmCommand(void)
{

    alarm_speaker_args.alarm_speaker_control = arg_int0("s", NULL, "<bool>", "Turn alarm On/Off");
    alarm_speaker_args.alarm_frequency = arg_int0("f", NULL, "<int>", "Set Alarm Frequency");
    alarm_speaker_args.alarm_duty_cycle = arg_int0("d", NULL, "<int>", "Set Alarm Duty Cycle");
    alarm_speaker_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "alrm",
        .help = "Alarm Speaker Commands",
        .hint = NULL,
        .func = &alarm_speaker,
        .argtable = &alarm_speaker_args};

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}