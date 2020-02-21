
#include "RegisterCommands.h"


static void register_version(void);
static void register_arguments(void);
static void register_time(void);
static void register_blink(void);
static void register_getdatetime(void);

void register_system(void)
{
    register_version();
    register_arguments();
    register_time();
    register_blink();
    register_getdatetime();
}


void register_queues(void)
{
    blink_queue =  xQueueCreate(3, 1);
    rtc_command_queue = xQueueCreate(3, sizeof(COMMAND_MESSAGE_STRUCT)); 

}




/* 'version' command */
static int get_version(int argc, char **argv)
{
    esp_chip_info_t info;
    esp_chip_info(&info);
    printf("IDF Version:%s\r\n", esp_get_idf_version());
    printf("Chip info:\r\n");
    printf("\tmodel:%s\r\n", info.model == CHIP_ESP32 ? "ESP32" : "Unknow");
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
    esp_console_cmd_t cmd;
    cmd.command = "version";
    cmd.help = "Get version of chip and SDK";
    cmd.hint = NULL;
    cmd.func = &get_version;

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}


static struct {
    struct arg_lit *up;
    struct arg_lit *down;
    struct arg_lit *left;
    struct arg_lit *right;
    struct arg_end *end;
} arguments_args;

static int arguments(int argc, char**argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &arguments_args);

    if (nerrors != 0) 
    {
        arg_print_errors(stderr, arguments_args.end, argv[0]);
        return 1;
    }

    if (arguments_args.up->count)
    {
        printf("Up!\n");
    }
    else if (arguments_args.down->count)
    {
        printf("Down!\n");
    }
    else if (arguments_args.left->count)
    {
        printf("Left!\n");
    }
    else if (arguments_args.right->count)
    {
        printf("Right!\n");
    }
    return 0;
}

static void register_arguments(void)
{
    arguments_args.up    = arg_lit0("u",  "up",    "Go UP!");
    arguments_args.down  = arg_lit0("d",  "down",  "Go DOWN!");
    arguments_args.left  = arg_lit0("l",  "left",  "Go LEFT!");
    arguments_args.right = arg_lit0("r",  "right", "Go RIGHT!");
    arguments_args.end   = arg_end(4);

    esp_console_cmd_t cmd;
    cmd.command = "direction";
    cmd.help = "Move a direction!";
    cmd.hint = NULL;
    cmd.func = &arguments;
    cmd.argtable = &arguments_args;

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));

}




static struct {
    struct arg_int *seconds;
    struct arg_int *minutes;
    struct arg_int *hours;
    struct arg_int *hour_12_N24;
    struct arg_end *end;
} time_args;

static int set_datetime(int argc, char**argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &time_args);

    if (nerrors != 0) 
    {
        arg_print_errors(stderr, time_args.end, argv[0]);
        return 1;
    }

    if (time_args.seconds->count)
    {
        printf("Seconds: %d\n", time_args.seconds->ival[0]);
    }
    else if (time_args.minutes->count)
    {
        printf("Minutes: %d\n", time_args.minutes->ival[0]);
    }
    else if (time_args.hours->count)
    {
        printf("Hours: %d\n", time_args.hours->ival[0]);
    }
    else if (time_args.hour_12_N24->count)
    {
        if (time_args.hour_12_N24->ival[0])
        {
            printf("12 Hour Clock\n");
        }
        else
        {
            printf("24 Hour Clock\n");
        }
        
    }
    return 0;
}

static void register_time(void)
{
    time_args.seconds     = arg_int0("s",  "seconds", "<s>", "Set seconds!");
    time_args.minutes     = arg_int0("m",  "minutes", "<m>", "Set minutes!");
    time_args.hours       = arg_int0("h",  "hours",   "<h>", "Set hours!");
    time_args.hour_12_N24 = arg_int0("f",  "format",  "<0|1>", "Set clock format!");
    time_args.end         = arg_end(4);

    esp_console_cmd_t cmd;
    cmd.command = "time";
    cmd.help = "Set the date time!";
    cmd.hint = NULL;
    cmd.func = &set_datetime;
    cmd.argtable = &time_args;

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));

}



static int blink_led(int argc, char **argv)
{
   char msg = 'b';
   xQueueSend(blink_queue, &msg, 30);
   return 0;
}


static void register_blink(void)
{
    const esp_console_cmd_t cmd = {
        .command = "blink",
        .help = "Blink LED",
        .hint = NULL,
        .func = &blink_led,
    };

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}




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