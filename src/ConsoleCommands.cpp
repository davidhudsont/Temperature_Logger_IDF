#include "ConsoleCommands.h"
#include "esp_log.h"
#include "esp_console.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "argtable3/argtable3.h"
#include "DeviceCommands.h"

static void register_version(void);
static void register_time(void);
static void register_date(void);
static void register_getdatetime(void);
static void register_temperature(void);
static void register_adjust_log_level_command(void);
static void register_lcd_command(void);

// cppcheck-suppress unusedFunction
void register_system(void)
{
    register_version();
    register_time();
    register_date();
    register_getdatetime();
    register_temperature();
    register_adjust_log_level_command();
    register_lcd_command();
}

static int get_version(int argc, char **argv)
{
    esp_chip_info_t info;
    esp_chip_info(&info);
    printf("IDF Version:%s\r\n", esp_get_idf_version());
    printf("Chip info:\r\n");
    printf("\tmodel:%s\r\n", info.model == CHIP_ESP32 ? "ESP32" : "Unknown");
    printf("\tcores:%d\r\n", info.cores);
    printf("\tfeature:%s%s%s%s%d%s\r\n",
           info.features & CHIP_FEATURE_WIFI_BGN ? "/802.11bgn" : "",
           info.features & CHIP_FEATURE_BLE ? "/BLE" : "",
           info.features & CHIP_FEATURE_BT ? "/BT" : "",
           info.features & CHIP_FEATURE_EMB_FLASH ? "/Embedded-Flash:" : "/External-Flash:",
           (int)spi_flash_get_chip_size() / (1024 * 1024), " MB");
    printf("\trevision number:%d\r\n", info.revision);
    return 0;
}

static void register_version(void)
{
    const esp_console_cmd_t cmd = {
        .command = "version",
        .help = "Get version of chip and SDK",
        .hint = NULL,
        .func = &get_version,
        .argtable = NULL,
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
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
        printf("Set Seconds to: %d\n", time_args.seconds->ival[0]);
        setSeconds(seconds);
    }
    else if (time_args.minutes->count)
    {
        uint8_t minutes = time_args.minutes->ival[0];
        printf("Minutes: %d\n", minutes);
        setMinutes(minutes);
    }
    else if (time_args.hours12->count)
    {
        bool PM_notAM = time_args.hours12->ival[1];
        int hours = time_args.hours12->ival[0];
        printf("Hours: %d, %s\n", hours, (PM_notAM ? "PM" : "AM"));
        setHours12Mode(hours, PM_notAM);
    }
    else if (time_args.hours24->count)
    {
        int hours = time_args.hours24->ival[0];
        printf("Hours: %d\n", hours);
        setHours24Mode(hours);
    }

    return 0;
}

static void register_time(void)
{
    time_args.seconds = arg_int0("s", "seconds", "<s>", "Set seconds!");
    time_args.minutes = arg_int0("m", "minutes", "<m>", "Set minutes!");
    time_args.hours12 = arg_intn("h", "hours12", "<h>, <0 AM|1 PM>", 0, 2, "Set hours in 12 hour format!");
    time_args.hours24 = arg_int0("t", "hours24", "<h>", "Set hours in 24 hour format!");
    time_args.end = arg_end(4);

    const esp_console_cmd_t cmd = {
        .command = "time",
        .help = "Set the time!",
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
        printf("Set Days to: %d\n", date_args.days->ival[0]);
        setWeekDay(days);
    }
    else if (date_args.date->count)
    {
        int dayOfMonth = date_args.date->ival[0];
        printf("Set Date to: %d\n", dayOfMonth);
        setDayOfMonth(dayOfMonth);
    }
    else if (date_args.month->count)
    {
        int month = date_args.month->ival[0];
        printf("Set Month to: %d\n", month);
        setMonth(month);
    }
    else if (date_args.year->count)
    {
        int year = date_args.year->ival[0];
        printf("Set Year to: %d\n", year);
        setYear(year);
    }

    return 0;
}

static void register_date(void)
{
    date_args.days = arg_int0("w", NULL, "<w>", "Set weekday!");
    date_args.date = arg_int0("d", NULL, "<d>", "Set the date!");
    date_args.month = arg_int0("m", NULL, "<m>", "Set the months!");
    date_args.year = arg_int0("y", NULL, "<y>", "Set the year!");
    date_args.end = arg_end(4);

    const esp_console_cmd_t cmd = {
        .command = "date",
        .help = "Set the date!",
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

static void register_getdatetime(void)
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

static void register_temperature(void)
{
    temperature_args.tempf = arg_lit0("f", NULL, "Get temperature in Fahrenheit!");
    temperature_args.tempc = arg_lit0("c", NULL, "Get temperature in Celsius!");
    temperature_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "temperature",
        .help = "Print the Temperature",
        .hint = NULL,
        .func = &get_temperature,
        .argtable = &temperature_args,
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static struct
{
    struct arg_int *level;
    struct arg_str *tag;
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
            printf("Setting Log Level %d\n", loglevel);
        }
        else
        {
            printf("Invalid Log Level %d\n", loglevel);
        }
    }
    return 0;
}

static void register_adjust_log_level_command(void)
{
    level_args.level = arg_int0("l", NULL, "0..5", "Set the log level");
    level_args.tag = arg_str0("t", NULL, "string", "Set log level based on tag");
    level_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "loglevel",
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
            displayOn();
        else
            displayOff();
    }
    else if (lcd_args.contrast->count)
    {
        uint8_t contrast = lcd_args.contrast->ival[0];
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
        }
        else if (lcd_args.backlight->count == 2)
        {
            uint8_t r = lcd_args.backlight->ival[0];
            uint8_t g = lcd_args.backlight->ival[1];
            uint8_t b = 0;
            setBackLight(r, g, b);
        }
        else if (lcd_args.backlight->count == 3)
        {
            uint8_t r = lcd_args.backlight->ival[0];
            uint8_t g = lcd_args.backlight->ival[1];
            uint8_t b = lcd_args.backlight->ival[2];
            setBackLight(r, g, b);
        }
    }
    else if (lcd_args.clear->count)
    {
        clearDisplay();
    }

    return 0;
}

static void register_lcd_command(void)
{

    lcd_args.display_toggle = arg_int0("d", NULL, "<bool>", "Turn Display On/Off");
    lcd_args.contrast = arg_int0("c", NULL, "<0-255>", "Set the Contrast");
    lcd_args.backlight = arg_intn("b", NULL, "<0-255>r, <0-255>g, <0-255>b", 0, 3, "Set the backlight rgb");
    lcd_args.clear = arg_lit0("r", NULL, "Clear the Display");
    lcd_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "lcd",
        .help = "LCD Commands",
        .hint = NULL,
        .func = &lcd,
        .argtable = &lcd_args};

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}