
/**
 * @file ConsoleCommands.c
 * @author David Hudson
 * @brief Implementation file that contains all the functions
 *        to be registered as console commands.
 * @date 2020-02-21
 * 
 */

#include "ConsoleCommands.h"


static void register_version(void);
static void register_time(void);
static void register_getdatetime(void);

/**
 * @brief Register all the console commands
 * 
 */
void register_system(void)
{
    register_version();
    register_time();
    register_getdatetime();
}

/**
 * @brief Create the console command queues
 * 
 */
void register_queues(void)
{
    rtc_command_queue = xQueueCreate(3, sizeof(COMMAND_MESSAGE_STRUCT)); 

}


/**
 * @brief Get the version of the chip
 */
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

static struct {
    struct arg_int *seconds;
    struct arg_int *minutes;
    struct arg_int *hours12;
    struct arg_int *hours24;
    struct arg_end *end;
} time_args;


/**
 * @brief Set the time of the RTC.
 * 
 */
static int set_time(int argc, char**argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &time_args);
    COMMAND_MESSAGE_STRUCT msg;
    
    if (nerrors != 0) 
    {
        arg_print_errors(stderr, time_args.end, argv[0]);
        return 1;
    }

    if (time_args.seconds->count)
    {
        printf("Set Seconds to: %d\n", time_args.seconds->ival[0]);
        msg.id = 's';
        msg.arg1 = time_args.seconds->ival[0];
    }
    else if (time_args.minutes->count)
    {
        int minutes = time_args.minutes->ival[0];
        printf("Minutes: %d\n", minutes);
        msg.id = 'm';
        msg.arg1 = minutes;
    }
    else if (time_args.hours12->count)
    {
        bool PM_notAM = time_args.hours12->ival[1];
        int hours = time_args.hours12->ival[0];
        printf("Hours: %d, %s\n", hours, ( PM_notAM ? "PM" : "AM"));
        msg.id = 'h';
        msg.arg1 = hours;
        msg.arg2 = PM_notAM;
    }
    else if (time_args.hours24->count)
    {
        int hours = time_args.hours24->ival[0];
        printf("Hours: %d\n", hours);
        msg.id = 't';
        msg.arg1 = hours;
        
    }
    xQueueSend(rtc_command_queue, (void*)&msg, 30);
    return 0;
}

static void register_time(void)
{
    time_args.seconds = arg_int0("s",  "seconds", "<s>", "Set seconds!");
    time_args.minutes = arg_int0("m",  "minutes", "<m>", "Set minutes!");
    time_args.hours12 = arg_intn("h",  "hours12",   "<h>, <0 AM|1 PM>", 0,2, "Set hours in 12 hour format!");
    time_args.hours24 = arg_int0("t",  "hours24",  "<h>", "Set hours in 24 hour format!");
    time_args.end     = arg_end(4);

    const esp_console_cmd_t cmd = {
        .command = "time",
        .help = "Set the time!",
        .hint = NULL,
        .func = &set_time,
        .argtable = &time_args,
    };


    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));

}


/**
 * @brief Get the current date and time
 * 
 */
static int get_datetime(int argc, char **argv)
{
    COMMAND_MESSAGE_STRUCT msg;
    msg.id = 'p';
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