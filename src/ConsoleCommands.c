

#include "ConsoleCommands.h"

static QueueHandle_t rtc_command_queue;     // Queue to send device objects between tasks
static QueueHandle_t tmp_command_queue;     // Queue to send device objects between tasks
static QueueHandle_t openlog_command_queue; // Queue to send device objects between tasks

static void register_version(void);
static void register_time(void);
static void register_date(void);
static void register_getdatetime(void);
static void register_temperature(void);
static void register_openlog_control(void);

int recieve_rtc_command(COMMAND_MESSAGE_STRUCT *msg)
{
    return xQueueReceive(rtc_command_queue, msg, 30);
}
int recieve_tmp_command(COMMAND_MESSAGE_STRUCT *msg)
{
    return xQueueReceive(tmp_command_queue, msg, 30);
}
int recieve_openlog_command(COMMAND_MESSAGE_STRUCT *msg)
{
    return xQueueReceive(openlog_command_queue, msg, 30);
}

/**
 * @brief Register all the console commands
 */
void register_system(void)
{
    register_version();
    register_time();
    register_date();
    register_getdatetime();
    register_temperature();
    register_openlog_control();
}

/**
 * @brief Create the console command queues
 */
void register_queues(void)
{
    rtc_command_queue = xQueueCreate(3, sizeof(COMMAND_MESSAGE_STRUCT));
    tmp_command_queue = xQueueCreate(3, sizeof(COMMAND_MESSAGE_STRUCT));
    openlog_command_queue = xQueueCreate(3, sizeof(COMMAND_MESSAGE_STRUCT));
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
           spi_flash_get_chip_size() / (1024 * 1024), " MB");
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
    COMMAND_MESSAGE_STRUCT msg;

    if (nerrors != 0)
    {
        arg_print_errors(stderr, time_args.end, argv[0]);
        return 1;
    }

    if (time_args.seconds->count)
    {
        printf("Set Seconds to: %d\n", time_args.seconds->ival[0]);
        msg.id = COMMAND_SET_SECONDS;
        msg.arg1 = time_args.seconds->ival[0];
    }
    else if (time_args.minutes->count)
    {
        int minutes = time_args.minutes->ival[0];
        printf("Minutes: %d\n", minutes);
        msg.id = COMMAND_SET_MINUTES;
        msg.arg1 = minutes;
    }
    else if (time_args.hours12->count)
    {
        bool PM_notAM = time_args.hours12->ival[1];
        int hours = time_args.hours12->ival[0];
        printf("Hours: %d, %s\n", hours, (PM_notAM ? "PM" : "AM"));
        msg.id = COMMAND_SET_12HOURS;
        msg.arg1 = hours;
        msg.arg2 = PM_notAM;
    }
    else if (time_args.hours24->count)
    {
        int hours = time_args.hours24->ival[0];
        printf("Hours: %d\n", hours);
        msg.id = COMMAND_SET_24HOURS;
        msg.arg1 = hours;
    }
    xQueueSend(rtc_command_queue, (void *)&msg, 30);
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
    COMMAND_MESSAGE_STRUCT msg;

    if (nerrors != 0)
    {
        arg_print_errors(stderr, date_args.end, argv[0]);
        return 1;
    }

    if (date_args.days->count)
    {
        printf("Set Days to: %d\n", date_args.days->ival[0]);
        msg.id = COMMAND_SET_WEEKDAY;
        msg.arg1 = date_args.days->ival[0];
    }
    else if (date_args.date->count)
    {
        int date = date_args.date->ival[0];
        printf("Set Date to: %d\n", date);
        msg.id = COMMAND_SET_DATE;
        msg.arg1 = date;
    }
    else if (date_args.month->count)
    {
        int month = date_args.month->ival[0];
        printf("Set Month to: %d\n", month);
        msg.id = COMMAND_SET_MONTH;
        msg.arg1 = month;
    }
    else if (date_args.year->count)
    {
        int year = date_args.year->ival[0];
        printf("Set Year to: %d\n", year);
        msg.id = COMMAND_SET_YEAR;
        msg.arg1 = year;
    }
    xQueueSend(rtc_command_queue, (void *)&msg, 30);
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
        .func = &set_date,
        .argtable = &date_args,
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static int get_datetime(int argc, char **argv)
{
    COMMAND_MESSAGE_STRUCT msg;
    msg.id = COMMAND_GET_DATETIME;
    xQueueSend(rtc_command_queue, (void *)&msg, 30);
    return 0;
}

static void register_getdatetime(void)
{
    const esp_console_cmd_t cmd = {
        .command = "datetime",
        .help = "Print the Date & Time",
        .hint = NULL,
        .func = &get_datetime,
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
    COMMAND_MESSAGE_STRUCT msg;

    if (nerrors != 0)
    {
        arg_print_errors(stderr, temperature_args.end, argv[0]);
        return 1;
    }

    if (temperature_args.tempf->count)
    {
        msg.id = COMMAND_GET_TEMPF;
    }
    if (temperature_args.tempc->count)
    {
        msg.id = COMMAND_GET_TEMPC;
    }

    xQueueSend(tmp_command_queue, (void *)&msg, 30);
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
    struct arg_int *stop_log;
    struct arg_end *end;
} openlog_args;

static int openlog_control(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&openlog_args);
    COMMAND_MESSAGE_STRUCT msg;

    if (nerrors != 0)
    {
        arg_print_errors(stderr, openlog_args.end, argv[0]);
        return 1;
    }

    if (openlog_args.stop_log->count)
    {
        msg.id = COMMAND_STOP_LOG;
        msg.arg1 = openlog_args.stop_log->ival[0];
    }

    xQueueSend(openlog_command_queue, (void *)&msg, 30);
    return 0;
}

static void register_openlog_control(void)
{
    openlog_args.stop_log = arg_int0("s", NULL, "<1|0>", "Stop and Start logging");
    openlog_args.end = arg_end(1);

    const esp_console_cmd_t cmd = {
        .command = "log",
        .help = "Control the logger",
        .hint = NULL,
        .func = &openlog_control,
        .argtable = &openlog_args,
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}