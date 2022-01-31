
// Standard Libraries
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>

// RTOS
#include "esp_console.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "linenoise/linenoise.h"
#include "sdkconfig.h"
#include "esp_timer.h"

// User Headers
#include "Console.h"
#include "Button.h"
#include "DEADONRTC.h"
#include "DeviceCommands.h"
#include "HMI.h"
#include "LCD.h"
#include "SystemTasks.h"
#include "TMP102.h"
#include "Speaker.h"

static SemaphoreHandle_t alarm_semiphore;
static SemaphoreHandle_t lcd_semiphore;

static DATE_TIME dateTime;
static float temperatureF;
static float temperatureC;

static void tmp102_task(void *pvParameter);
static void rtc_task(void *pvParameter);
static void button_task(void *pvParameter);
static void hmi_task(void *pvParameter);
static void speaker_task(void *pvParameter);

void CreateSemaphores(void)
{
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

void CreateTasks(void)
{
    gpio_install_isr_service(0);
    // Larger number equals higher priority
    xTaskCreate(&rtc_task, "RTC_Task", configMINIMAL_STACK_SIZE * 4, NULL, 4, NULL);
    xTaskCreate(&tmp102_task, "TMP102_Task", configMINIMAL_STACK_SIZE * 7, NULL, 5, NULL);
    xTaskCreate(&console_task, "Console_Task", configMINIMAL_STACK_SIZE * 5, NULL, 7, NULL);
    xTaskCreate(&hmi_task, "HMI Task", configMINIMAL_STACK_SIZE * 5, NULL, 3, NULL);
    xTaskCreate(&button_task, "Button_Task", configMINIMAL_STACK_SIZE * 4, NULL, 8, NULL);
    xTaskCreate(&speaker_task, "Speaker_Task", configMINIMAL_STACK_SIZE * 4, NULL, 3, NULL);
}

void PowerOnTest(RTCDS3234 &rtc)
{
    uint8_t code[] = {0x12, 0xF3, 0xBF, 0x65, 0x89, 0x90};
    uint8_t data[6] = {0};
    // Check for power lost
    rtc.SRAMBurstRead(0x00, data, 6);
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
        rtc.SRAMBurstWrite(0x00, code, 6);
        rtc.WriteBuildDateTime();
    }
    else
    {
        ESP_LOGI("RTC", "Power Not Lost");
    }
}

void StartAlarms(RTCDS3234 &rtc)
{
    // Setup the RTC interrupts
    rtc.ISRInitialize();
    delay(1000);
    rtc.WriteAlarm1(10, 0, 0, 0, ALARM1_SECONDS_MATCH);
    rtc.WriteAlarm2(0, 0, 0, ALARM2_PER_MIN);
    delay(100);
    rtc.EnableInterrupt(true);
    delay(100);
    rtc.EnableAlarms(false, true);
    // Clear the ALARM flags early
    rtc.ReadAlarm1Flag();
    rtc.ReadAlarm2Flag();
}

static void rtc_task(void *pvParameter)
{
    ESP_LOGI("RTC", "RTC Task Start!");
    RTCDS3234 rtc;
    COMMAND_MESSAGE cmd_msg;
    rtc.Begin();

    PowerOnTest(rtc);
    StartAlarms(rtc);

    while (1)
    {
        // Evaluate Alarm Interrupts
        if (GetInterruptSemiphore())
        {
            bool alarm1_flag = rtc.ReadAlarm1Flag();
            bool alarm2_flag = rtc.ReadAlarm2Flag();

            if (alarm1_flag)
            {
                ESP_LOGV("RTC", "ALARM1 Triggered");
                rtc.ReadDateTime();
                dateTime = rtc.GetDateTime();
                std::string logdate = rtc.DateToString();
                std::string logtime = rtc.TimeToString();
                ESP_LOGI("RTC", "%s, %s", logdate.c_str(), logtime.c_str());
                setAlarm(true);
            }
            if (alarm2_flag)
            {
                ESP_LOGV("RTC", "ALARM2 Triggered");
                rtc.ReadDateTime();
                dateTime = rtc.GetDateTime();
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
                rtc.ReadDateTime();
                std::string logdate = rtc.DateToString();
                std::string logtime = rtc.TimeToString();
                ESP_LOGI("RTC", "%s, %s", logdate.c_str(), logtime.c_str());
                dateTime = rtc.GetDateTime();
                break;
            }
            case SET_SECONDS:
                rtc.WriteSeconds(cmd_msg.arg1);
                dateTime.second = cmd_msg.arg1;
                break;
            case SET_MINUTES:
                rtc.WriteMinutes(cmd_msg.arg1);
                dateTime.minute = cmd_msg.arg1;
                break;
            case SET_12HOURS:
                rtc.Write12Hours(cmd_msg.arg1, cmd_msg.arg2);
                dateTime.hour = cmd_msg.arg1;
                break;
            case SET_24HOURS:
                rtc.Write24Hours(cmd_msg.arg1);
                dateTime.hour = cmd_msg.arg1;
                break;
            case SET_TIME:
                rtc.WriteTime(cmd_msg.arg1, cmd_msg.arg2, cmd_msg.arg3);
                dateTime.hour = cmd_msg.arg1;
                dateTime.minute = cmd_msg.arg2;
                dateTime.second = cmd_msg.arg3;
                break;
            case SET_WEEKDAY:
                rtc.WriteDays((DAYS)cmd_msg.arg1);
                break;
            case SET_DAYOFMONTH:
                rtc.WriteDate(cmd_msg.arg1);
                dateTime.dayofMonth = cmd_msg.arg1;
                break;
            case SET_MONTH:
                rtc.WriteMonth(cmd_msg.arg1);
                dateTime.month = cmd_msg.arg1;
                break;
            case SET_YEAR:
                rtc.WriteYear(cmd_msg.arg1);
                dateTime.year = cmd_msg.arg1;
                break;
            case SET_DATE:
                rtc.WriteDate(cmd_msg.arg1, cmd_msg.arg2, cmd_msg.arg3);
                dateTime.dayofMonth = cmd_msg.arg1;
                dateTime.month = cmd_msg.arg2;
                dateTime.year = cmd_msg.arg3;
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
        ESP_LOGV("TMP", "Set the OneShot!");
        tmp102.SetOneShot();
        delay(30);
        oneshot = tmp102.GetOneShot();
    }

    if (oneshot)
    {
        tmp102.ReadTemperature();
        ESP_LOGV("TMP", "Temperature has been Read");
        oneshot = false;
    }
}

static void tmp102_task(void *pvParameter)
{
    TMP102 tmp102;
    COMMAND_MESSAGE cmd_msg;

    ESP_LOGI("TMP", "TMP102 Task Start!");
    tmp102.Begin();
    tmp102.SetConversionRate(CONVERSION_MODE_1);
    delay(100);
    tmp102.Sleep();
    delay(300);
    OneShotTemperatureRead(tmp102);
    OneShotTemperatureRead(tmp102);
    temperatureF = tmp102.TemperatureF();
    temperatureC = tmp102.Temperature();

    while (1)
    {
        if (xSemaphoreTake(alarm_semiphore, 0))
        {
            OneShotTemperatureRead(tmp102);
            std::string temperature_readingf = tmp102.TemperatureFToString();
            ESP_LOGI("TMP", "%sF", temperature_readingf.c_str());
        }

        if (recieveTMPCommand(&cmd_msg))
        {
            switch (cmd_msg.id)
            {
            case GET_TEMPF:
                OneShotTemperatureRead(tmp102);
                temperatureF = tmp102.TemperatureF();
                ESP_LOGI("TMP", "%3.3fC", temperatureF);
                break;
            case GET_TEMPC:
                OneShotTemperatureRead(tmp102);
                temperatureC = tmp102.Temperature();
                ESP_LOGI("TMP", "%2.3fC", temperatureC);
                break;
            default:
                break;
            }
            xSemaphoreGive(lcd_semiphore);
        }
    }
}

static void button_task(void *pvParameter)
{
    ESP_LOGI("BTN", "Starting Button Interface");
    Button altButton(GPIO_NUM_13);
    Button editModeButton(GPIO_NUM_12);
    Button downButton(GPIO_NUM_14);
    Button upButton(GPIO_NUM_27);
    while (true)
    {
        if (altButton)
        {
            ESP_LOGI("BTN", "Alt Button Pressed");
            buttonPressed(ALT_BTN_PRESSED);
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

    readDateTime();

    while (1)
    {

        if (xSemaphoreTake(lcd_semiphore, 0) && hmi.getCurrentState() == DISPLAYING)
        {
            hmi.setDisplayDateTime(dateTime);
            hmi.setDisplayTemperature(temperatureF, temperatureC);
            updateDisplay();
        }
        hmi.process();
    }
}

static void speaker_task(void *pvParameter)
{
    AlarmSpeaker alarm = AlarmSpeaker(GPIO_NUM_17);
    alarm.Init();

    while (1)
    {
        COMMAND_MESSAGE cmd_msg;
        if (recieveAlarmCommand(&cmd_msg))
        {
            if (cmd_msg.id == ALARM_SET)
            {
                if (cmd_msg.arg1)
                {
                    alarm.StartAlarm();
                }
                else
                {
                    alarm.StopAlarm();
                }
            }
            else if (cmd_msg.id == ALARM_FREQ)
            {
                alarm.SetFrequency((uint32_t)cmd_msg.arg1);
            }
            else if (cmd_msg.id == ALARM_DUTY_CYCLE)
            {
                alarm.SetDutyCyclePercentage((uint32_t)cmd_msg.arg1);
            }
        }
        if (recieveButtonCommand(&cmd_msg))
        {
            if (cmd_msg.id == ALT_BTN_PRESSED)
            {
                alarm.StopAlarm();
            }
        }
        delay(100);
    }
}
