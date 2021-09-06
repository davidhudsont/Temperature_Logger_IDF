
#include "ConsoleCommands.h"
#include "System_Tasks.h"
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "DEADONRTC.h"
#include "TMP102.h"
#include "LCD.h"
#include "bspConsole.h"
#include "linenoise/linenoise.h"
#include "esp_console.h"
#include <sys/unistd.h>
#include <sys/stat.h>
#include "Button.h"
#include "DeviceCommands.h"
#include "HMI.h"
#define DISABLE_SD_CARD
#ifndef DISABLE_SD_CARD
#include "BSP_SD.h"
#endif

static SemaphoreHandle_t log_semiphore;
static SemaphoreHandle_t alarm_semiphore;
static SemaphoreHandle_t lcd_semiphore;

static DATE_TIME dateTime;
static float temperatureF;
static float temperatureC;

static void tmp102_task(void *pvParameter);
static void rtc_intr_task(void *pvParameter);
static void console_task(void *pvParameter);
#ifndef DISABLE_SD_CARD
static void sdcard_task(void *pvParameter);
#endif
static void button_task(void *pvParameter);
static void hmi_task(void *pvParameter);

void Create_Semaphores(void)
{
    log_semiphore = xSemaphoreCreateBinary();
    alarm_semiphore = xSemaphoreCreateBinary();
    lcd_semiphore = xSemaphoreCreateBinary();
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

void Create_Tasks(void)
{
    gpio_install_isr_service(0);
    // Larger number equals higher priority
    xTaskCreate(&rtc_intr_task, "RTC_Task", configMINIMAL_STACK_SIZE * 4, NULL, 4, NULL);
    xTaskCreate(&tmp102_task, "TMP102_Task", configMINIMAL_STACK_SIZE * 7, NULL, 5, NULL);
    xTaskCreate(&console_task, "Console_Task", configMINIMAL_STACK_SIZE * 5, NULL, 7, NULL);
    xTaskCreate(&hmi_task, "HMI Task", configMINIMAL_STACK_SIZE * 5, NULL, 3, NULL);
    xTaskCreate(&button_task, "Button_Task", configMINIMAL_STACK_SIZE * 4, NULL, 8, NULL);

#ifndef DISABLE_SD_CARD
    xTaskCreate(&sdcard_task, "SDCard_Task", configMINIMAL_STACK_SIZE * 4, NULL, 6, NULL);
#endif
}

#ifndef DISABLE_SD_CARD
static void sdcard_task(void *pvParameter)
{
    BSP::SD sd;
    sd.Mount();
    std::string file_name = "TLOG.csv";

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
            }
            else if (msg.id == COMMAND_START_LOG)
            {
                ESP_LOGI("LOG", "Started Logging");
                sd.OpenFile(file_name);
                if (sd.IsFileOpen())
                {
                    std::string column_names = "Date, Time, Temperature(F)";
                    sd.WriteLine(column_names);
                }
            }
            else if (msg.id == COMMAND_STOP_LOG)
            {
                ESP_LOGI("LOG", "Stopped Logging");
                sd.CloseFile();
            }
            else if (msg.id == COMMAND_DELETE_LOG)
            {
                ESP_LOGI("LOG", "Deleting Logging File and stopped Logging");
                if (sd.IsFileOpen())
                {
                    sd.CloseFile();
                }
                sd.DeleteFile(file_name);
            }
        }
        if (xSemaphoreTake(log_semiphore, 0))
        {
            std::string logline = logdate + ", " + logtime + ", " + temperature_reading;
            ESP_LOGI("LOG", "%s", logline.c_str());
            if (sd.IsFileOpen())
            {
                ESP_LOGI("LOG", "Logging to SD Card");
                sd.WriteLine(logline);
            }
        }
    }
}
#endif

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
    COMMAND_MESSAGE cmd_msg;
    rtc.Begin();

    Power_On_Test(rtc);
    Start_Alarms(rtc);

    while (1)
    {
        // Evaluate Alarm Interrupts
        if (GetInterruptSemiphore())
        {
            bool alarm1_flag = rtc.READ_ALARM1_FLAG();
            bool alarm2_flag = rtc.READ_ALARM2_FLAG();

            if (alarm1_flag)
            {
                ESP_LOGI("RTC", "ALARM1 Triggered");
                rtc.READ_DATETIME();
                dateTime = rtc.GET_DATETIME();
            }
            if (alarm2_flag)
            {
                ESP_LOGI("RTC", "ALARM2 Triggered");
                rtc.READ_DATETIME();
                dateTime = rtc.GET_DATETIME();
                xSemaphoreGive(alarm_semiphore);
                xSemaphoreGive(lcd_semiphore);
            }
        }

        // Evaulate Console Commands
        if (recieveDateCommand(&cmd_msg) || recieveTimeCommand(&cmd_msg))
        {
            switch (cmd_msg.id)
            {
            case GET_DATETIME:
            {
                rtc.READ_DATETIME();
                std::string logdate = rtc.DATE_TOSTRING();
                std::string logtime = rtc.TIME_TOSTRING();
                ESP_LOGI("RTC", "%s, %s", logdate.c_str(), logtime.c_str());
                dateTime = rtc.GET_DATETIME();
                break;
            }
            case SET_SECONDS:
                rtc.WRITE_SECONDS(cmd_msg.arg1);
                break;
            case SET_MINUTES:
                rtc.WRITE_MINUTES(cmd_msg.arg1);
                break;
            case SET_12HOURS:
                rtc.WRITE_12HOURS(cmd_msg.arg1, cmd_msg.arg2);
                break;
            case SET_24HOURS:
                rtc.WRITE_24HOURS(cmd_msg.arg1);
                break;
            case SET_WEEKDAY:
                rtc.WRITE_DAYS((DAYS)cmd_msg.arg1);
                break;
            case SET_DAYOFMONTH:
                rtc.WRITE_DATE(cmd_msg.arg1);
                dateTime.dayofMonth = cmd_msg.arg1;
                break;
            case SET_MONTH:
                rtc.WRITE_MONTH(cmd_msg.arg1);
                dateTime.month = cmd_msg.arg1;
                break;
            case SET_YEAR:
                rtc.WRITE_YEAR(cmd_msg.arg1);
                dateTime.year = cmd_msg.arg1;
                break;
            default:
                break;
            }
            xSemaphoreGive(lcd_semiphore);
        }
    }
}

static void OneShotTemperatureRead(TMP102 &tmp102)
{
    static bool oneshot = false;
    if (oneshot == false)
    {
        ESP_LOGI("TMP", "Set the OneShot!");
        tmp102.Set_OneShot();
        delay(30);
        oneshot = tmp102.Get_OneShot();
    }

    if (oneshot)
    {
        tmp102.Read_Temperature();
        ESP_LOGI("TMP", "Temperature has been Read");
        oneshot = false;
    }
}

static void tmp102_task(void *pvParameter)
{
    TMP102 tmp102;
    COMMAND_MESSAGE cmd_msg;

    ESP_LOGI("TMP", "TMP102 Task Start!");
    tmp102.Begin();
    tmp102.Set_Conversion_Rate(CONVERSION_MODE_1);
    delay(100);
    tmp102.Sleep();
    delay(300);
    OneShotTemperatureRead(tmp102);
    OneShotTemperatureRead(tmp102);
    temperatureF = tmp102.Get_TemperatureF();
    temperatureC = tmp102.Get_Temperature();

    while (1)
    {
        if (xSemaphoreTake(alarm_semiphore, 0))
        {
            OneShotTemperatureRead(tmp102);
            std::string temperature_readingf = tmp102.Get_TemperatureF_ToString();
            ESP_LOGI("TMP", "%sF", temperature_readingf.c_str());
            xSemaphoreGive(log_semiphore);
        }

        if (recieveTMPCommand(&cmd_msg))
        {
            switch (cmd_msg.id)
            {
            case GET_TEMPF:
                OneShotTemperatureRead(tmp102);
                temperatureF = tmp102.Get_TemperatureF();
                ESP_LOGI("TMP", "%3.3fC", temperatureF);
                break;
            case GET_TEMPC:
                OneShotTemperatureRead(tmp102);
                temperatureC = tmp102.Get_Temperature();
                ESP_LOGI("TMP", "%2.3fC", temperatureC);
                break;
            default:
                break;
            }
            xSemaphoreGive(lcd_semiphore);
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

static void button_task(void *pvParameter)
{
    ESP_LOGI("BTN", "Starting Button Interface");
    Button editSettingButton(GPIO_NUM_13);
    Button editModeButton(GPIO_NUM_12);
    Button downButton(GPIO_NUM_14);
    Button upButton(GPIO_NUM_27);
    while (true)
    {
        if (editSettingButton)
        {
            ESP_LOGI("BTN", "Edit Setting Button Pressed");
            buttonPressed(EDIT_SETTING_PRESSED);
        }
        else if (editModeButton)
        {
            ESP_LOGI("BTN", "Edit Mode Button Pressed");
            buttonPressed(EDIT_MODE_PRESSED);
        }
        else if (downButton)
        {
            ESP_LOGI("BTN", "Down Button Pressed");
            buttonPressed(DOWN_PRESSED);
        }
        else if (upButton)
        {
            ESP_LOGI("BTN", "Up Button Pressed");
            buttonPressed(UP_PRESSED);
        }
        delay(10);
    }
}

static void hmi_task(void *pvParameter)
{
    HMI hmi = HMI();
    while (1)
    {

        if (xSemaphoreTake(lcd_semiphore, 0))
        {
            hmi.setDisplayDateTime(dateTime);
            hmi.setDisplayTemperature(temperatureF, temperatureC);
            updateDisplay();
        }
        hmi.process();
    }
}
