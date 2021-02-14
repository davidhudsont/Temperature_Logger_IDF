
#include "ConsoleCommands.h"
#include "System_Tasks.h"
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "DEADONRTC.h"
#include "TMP102.h"
#include "bspConsole.h"
#include "linenoise/linenoise.h"
#include "esp_console.h"
#include "BSP_SD.h"
#include <sys/unistd.h>
#include <sys/stat.h>

static const char *SDTAG = "SDCard";
static std::string temperature;
static std::string datetime;

// Used to communicate between tasks
typedef struct MESSAGE_STRUCT
{
    char id;
    void *device;
} MESSAGE_STRUCT;

static QueueHandle_t device_queue; // Queue to send device objects between tasks

static QueueHandle_t alarm_queue; // Sends message when an alarm has been triggered

static void tmp102_sleep_task(void *pvParameter);
static void rtc_intr_task(void *pvParameter);
static void console_task(void *pvParameter);
static void sdcard_task(void *pvParameter);

void Create_Task_Queues(void)
{
    device_queue = xQueueCreate(3, sizeof(MESSAGE_STRUCT));
    alarm_queue = xQueueCreate(3, 1);
    register_queues();
}

void Create_Tasks(void)
{
    xTaskCreate(&rtc_intr_task, "rtc_intr_task", configMINIMAL_STACK_SIZE * 3, NULL, 4, NULL);
    xTaskCreate(&tmp102_sleep_task, "tmp102sleep_task", configMINIMAL_STACK_SIZE * 7, NULL, 5, NULL);
    xTaskCreate(&console_task, "console_task", configMINIMAL_STACK_SIZE * 4, NULL, 7, NULL);
    xTaskCreate(&sdcard_task, "sdcard_task", configMINIMAL_STACK_SIZE * 4, NULL, 6, NULL);
}

/**
 * @brief Delays a task for the passed
 *        in parameter time_ms in milliseconds
 * @param time_ms 
 */
void delay(uint32_t time_ms)
{
    vTaskDelay(time_ms / portTICK_PERIOD_MS);
}

/**
 * @brief Print the current datetime.
 * @param rtc - DEADONTRC device structure
 */
void Print_DateTime(RTCDS3234 &rtc)
{
    uint8_t hours = rtc.hours;
    uint8_t minutes = rtc.minutes;
    uint8_t seconds = rtc.seconds;
    uint8_t date = rtc.date;
    uint8_t month = rtc.month;
    uint8_t year = rtc.year;

    if (rtc.hour12_not24)
    {
        bool PM_notAM = rtc.PM_notAM;
        ESP_LOGI("RTC", "%02d:%02d:%02d %s, %02d-%02d-%04d", hours, minutes, seconds, (PM_notAM ? "PM" : "AM"), month, date, year + 2000);
    }
    else
    {
        ESP_LOGI("RTC", "%02d:%02d:%02d, %02d-%02d-%04d", hours, minutes, seconds, month, date, year + 2000);
    }
}

static void exampleFileTest()
{
    ESP_LOGI(SDTAG, "Opening file");
    FILE *f = fopen(MOUNT_POINT "/hello.txt", "w");
    if (f == NULL)
    {
        ESP_LOGE(SDTAG, "Failed to open file for writing");
    }
    else
    {
        fprintf(f, "Hello World!\n");
        fclose(f);
        ESP_LOGI(SDTAG, "File written");
        // Check if destination file exists before renaming
        struct stat st;
        if (stat(MOUNT_POINT "/foo.txt", &st) == 0)
        {
            // Delete it if it exists
            unlink(MOUNT_POINT "/foo.txt");
        }

        // Rename original file
        ESP_LOGI(SDTAG, "Renaming file");
        if (rename(MOUNT_POINT "/hello.txt", MOUNT_POINT "/foo.txt") != 0)
        {
            ESP_LOGE(SDTAG, "Rename failed");
        }

        // Open renamed file for reading
        ESP_LOGI(SDTAG, "Reading file");
        f = fopen(MOUNT_POINT "/foo.txt", "r");
        if (f == NULL)
        {
            ESP_LOGE(SDTAG, "Failed to open file for reading");
        }
        char line[64];
        fgets(line, sizeof(line), f);
        fclose(f);
        // strip newline
        char *pos = strchr(line, '\n');
        if (pos)
        {
            *pos = '\0';
        }
        ESP_LOGI(SDTAG, "Read from file: '%s'\n", line);
    }
}

static void sdcard_task(void *pvParameter)
{
    BSP::SD sd;
    sd.Mount();
    std::string file_name = "TLOG.txt";

    while (1)
    {
        COMMAND_MESSAGE_STRUCT msg;
        if (recieve_sdcard_command(&msg))
        {
            if (msg.id == COMMAND_GET_DISK)
            {
                sd.PrintDiskInfo();
            }
            else if (msg.id == COMMAND_WRITE_DISK)
            {
                exampleFileTest();
            }
            else if (msg.id == COMMAND_START_LOG)
            {
                sd.OpenFile(file_name);
            }
            else if (msg.id == COMMAND_STOP_LOG)
            {
                sd.CloseFile();
            }
        }
    }
}

void Power_On_Test(RTCDS3234 &rtc)
{
    uint8_t code[] = {0x12, 0xF3, 0xBF, 0x65, 0x89, 0x90};
    uint8_t data[6] = {0};
    // Check for power lost
    rtc.SRAM_Burst_Read(0x00, data, 6);
    bool power_lost = false;
    for (int i = 0; i < 6; i++)
    {
        if (data[i] != code[i])
        {
            power_lost = true;
            break;
        }
    }
    // If power was lost write the code to the sram
    if (power_lost)
    {
        ESP_LOGW("RTC", "Lost Power!");
        rtc.SRAM_Burst_Write(0x00, code, 6);
        rtc.WRITE_BUILD_DATETIME();
    }
    else
    {
        ESP_LOGI("RTC", "Power Not Lost");
    }
}

void Start_Alarms(RTCDS3234 &rtc)
{
    // Setup the RTC interrupts
    rtc.ISR_Init();
    delay(1000);
    rtc.WRITE_ALARM1(10, 0, 0, 0, ALARM1_SECONDS_MATCH);
    rtc.WRITE_ALARM2(0, 0, 0, ALARM2_PER_MIN);
    delay(100);
    rtc.Enable_Interrupt(true);
    delay(100);
    rtc.Enable_Alarms(true, true);
    // Clear the ALARM flags early
    rtc.READ_ALARM1_FLAG();
    rtc.READ_ALARM2_FLAG();
}

static void rtc_intr_task(void *pvParameter)
{
    ESP_LOGI("RTC", "RTC Task Start!");
    RTCDS3234 rtc;
    char msg;
    COMMAND_MESSAGE_STRUCT cmd_msg;
    rtc.Begin();

    Power_On_Test(rtc);
    Start_Alarms(rtc);

    while (1)
    {
        // Evaluate Alarm Interrupts
        if (get_queue(&msg))
        {
            if (msg == 'r')
            {
                bool alarm1_flag = rtc.READ_ALARM1_FLAG();
                bool alarm2_flag = rtc.READ_ALARM2_FLAG();

                if (alarm1_flag)
                {
                    ESP_LOGI("RTC", "ALARM1 Triggered");
                    rtc.READ_DATETIME();
                    datetime = rtc.DATETIME_TOSTRING();
                    ESP_LOGI("RTC", "%s", datetime.c_str());
                }
                if (alarm2_flag)
                {
                    ESP_LOGI("RTC", "ALARM2 Triggered");
                    rtc.READ_DATETIME();
                    datetime = rtc.DATETIME_TOSTRING();
                    char tmp_ready = 'r';
                    xQueueSend(alarm_queue, (void *)&tmp_ready, 30);
                }
            }
        }

        // Evaulate Console Commands
        if (recieve_rtc_command(&cmd_msg))
        {
            switch (cmd_msg.id)
            {
            case COMMAND_GET_DATETIME:
            {
                rtc.READ_DATETIME();
                datetime = rtc.DATETIME_TOSTRING();
                ESP_LOGI("RTC", "%s", datetime.c_str());
            }
            break;
            case COMMAND_SET_SECONDS:
                rtc.WRITE_SECONDS(cmd_msg.arg1);
                break;
            case COMMAND_SET_MINUTES:
                rtc.WRITE_MINUTES(cmd_msg.arg1);
                break;
            case COMMAND_SET_12HOURS:
                rtc.WRITE_12HOURS(cmd_msg.arg1, cmd_msg.arg2);
                break;
            case COMMAND_SET_24HOURS:
                rtc.WRITE_24HOURS(cmd_msg.arg1);
                break;
            case COMMAND_SET_WEEKDAY:
                rtc.WRITE_DAYS((DAYS)cmd_msg.arg1);
                break;
            case COMMAND_SET_DATE:
                rtc.WRITE_DATE(cmd_msg.arg1);
                break;
            case COMMAND_SET_MONTH:
                rtc.WRITE_MONTH(cmd_msg.arg1);
                break;
            case COMMAND_SET_YEAR:
                rtc.WRITE_YEAR(cmd_msg.arg1);
                break;
            default:
                break;
            }
        }
    }
}

static void OneShotTemperatureRead(TMP102 &tmp102_device)
{
    static bool oneshot = false;
    if (oneshot == false)
    {
        ESP_LOGI("TMP", "Set the OneShot!");
        tmp102_device.Set_OneShot();
        delay(30);
        oneshot = tmp102_device.Get_OneShot();
    }

    if (oneshot)
    {
        tmp102_device.Read_Temperature();
        ESP_LOGI("TMP", "Temperature has been Read");
        oneshot = false;
    }
}

static void tmp102_sleep_task(void *pvParameter)
{
    TMP102 tmp102_device;
    char msg;
    COMMAND_MESSAGE_STRUCT cmd_msg;
    MESSAGE_STRUCT device_message;

    ESP_LOGI("TMP", "TMP102 Task Start!");
    tmp102_device.Begin();
    tmp102_device.Set_Conversion_Rate(CONVERSION_MODE_1);
    delay(100);
    tmp102_device.Sleep(true);
    delay(300);

    while (1)
    {

        if (xQueueReceive(alarm_queue, &msg, 30))
        {
            if (msg == 'r')
            {
                OneShotTemperatureRead(tmp102_device);
                device_message.id = 't';
                device_message.device = nullptr;
                xQueueSend(device_queue, &device_message, 30);
            }
        }

        if (recieve_tmp_command(&cmd_msg))
        {
            float temperature = 0;
            switch (cmd_msg.id)
            {
            case COMMAND_GET_TEMPF:
                OneShotTemperatureRead(tmp102_device);
                temperature = tmp102_device.Get_TemperatureF();
                ESP_LOGI("TMP", "%3.3fF", temperature);
                break;
            case COMMAND_GET_TEMPC:
                OneShotTemperatureRead(tmp102_device);
                temperature = tmp102_device.Get_Temperature();
                ESP_LOGI("TMP", "%2.3fC", temperature);
                break;
            default:
                break;
            }
        }
    }
}

static void console_task(void *pvParameter)
{
    Start_Console();

    Register_Console_Commands();

    const char *prompt = LOG_COLOR_I "esp> " LOG_RESET_COLOR;

    printf("\n"
           "***************************\n"
           "*      ESP 32 Console     *\n"
           "*    Temperature Logger   *\n"
           "***************************\n"
           "\n");

    printf("\n"
           "Type 'help' to get the list of commands.\n"
           "Use UP/DOWN arrows to navigate through command history.\n"
           "Press TAB when typing command name to auto-complete.\n");

    int probe_status = linenoiseProbe();
    if (probe_status)
    {
        printf("\n"
               "Your terminal application does not support escape sequences.\n"
               "Line editing and history features are disabled.\n"
               "On Windows, try using Putty instead.\n");
        linenoiseSetDumbMode(1);

        prompt = "esp32> ";
    }

    while (1)
    {
        char *line = linenoise(prompt);
        if (line == NULL)
        {
            continue;
        }

        linenoiseHistoryAdd(line);

        // Try to run a command
        int ret;
        esp_err_t err = esp_console_run(line, &ret);
        if (err == ESP_ERR_NOT_FOUND)
        {
            printf("Unrecognized Command\n");
        }
        else if (err == ESP_ERR_INVALID_ARG)
        {
        }
        else if (err == ESP_OK && ret != ESP_OK)
        {
            printf("Command returned non-zero error code: 0x%x (%s)\n", ret, esp_err_to_name(ret));
        }
        else if (err != ESP_OK)
        {
            printf("Internal error: %s\n", esp_err_to_name(err));
        }

        linenoiseFree(line);
    }
}
