/**
 * @file main.c
 * @author David Hudson
 * @brief Main Application for Temperature Logger
 * @date 2019-11-15
 * 
 * 
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "sdkconfig.h"
#include "bspSpi.h"
#include <string.h>
#include "DEADONRTC.h"
#include "TMP102.h"
#include "OPENLOG.h"
#include "bspConsole.h"
#include "esp_log.h"

/**
 * @brief Message sent between tasks
 * 
 */
typedef struct MESSAGE_STRUCT
{
    char id;
    void* device;
} MESSAGE_STRUCT;

#define BLINK_GPIO (5)

QueueHandle_t device_queue; // Queue to send device objects between tasks

QueueHandle_t alarm_queue; // Sends message when an alarm has been triggered

/**
 * @brief Delays a task for the passed
 *        in parameter time_ms in milliseconds
 * 
 * @param time_ms 
 */
void delay(uint32_t time_ms)
{
    vTaskDelay( time_ms / portTICK_PERIOD_MS);
}

/**
 * @brief Print the current datetime.
 * 
 * @param rtc - DEADONTRC device structure
 */
void Print_DateTime(DEADONRTC * rtc)
{
    uint8_t hours = rtc->hours;
    uint8_t minutes = rtc->minutes;
    uint8_t seconds = rtc->seconds;
    uint8_t date = rtc->date;
    uint8_t month = rtc->month;
    uint8_t year = rtc->year;

    if (rtc->hour12_not24)
    {
        bool PM_notAM = rtc->PM_notAM;
        printf("%02d:%02d:%02d %s, %02d-%02d-%04d\n",hours, minutes, seconds, (PM_notAM ? "PM" : "AM"), month, date, year+2000);    
    }
    else
    {
        printf("%02d:%02d:%02d, %02d-%02d-%04d\n",hours, minutes, seconds, month, date, year+2000);    
    }
    

}


void openlog_task_dummy(void *pvParameter)
{
    printf("OPENLOG Task Start!\n");
    MESSAGE_STRUCT * message_reciever;
    uint8_t * buffer = (uint8_t *) malloc(sizeof(MESSAGE_STRUCT));
    //char *line = (char *) malloc(500);

    while (1)
    {
        xQueueReceive(device_queue, buffer, 30);
        message_reciever = (MESSAGE_STRUCT *)buffer;

        if (message_reciever->id == 'r')
        {
            DEADONRTC * rtc = (DEADONRTC *)message_reciever->device;
            Print_DateTime(rtc);
            message_reciever->id = ' ';
        }
        else if (message_reciever->id == 't')
        {
            TMP102_STRUCT *tmp = (TMP102_STRUCT*) message_reciever->device;
            float temperature = TMP102_Get_Temperature(tmp);
            printf("Temperature = %f C\n", temperature);
            temperature = TMP102_Get_TemperatureF(tmp);
            printf("Temperature = %f F\n", temperature);
            message_reciever->id = ' ';
        }
        
    }

}

void openlog_task(void *pvParameter)
{
    printf("OPENLOG Task Start!\n");
    DEADONRTC rtc_dev;
    TMP102_STRUCT tmp102_dev;
    memset(&rtc_dev, 0, sizeof(DEADONRTC));
    memset(&tmp102_dev, 0, sizeof(TMP102_STRUCT));

    // Start the Openlog Device
    OPENLOG_STRUCT openlog_dev;
    OPENLOG_Begin(&openlog_dev);
    OPENLOG_UART_FLUSH(&openlog_dev);
    OPENLOG_EnterCommandMode(&openlog_dev);

    MESSAGE_STRUCT * message_reciever;
    uint8_t * buffer = (uint8_t *) malloc(sizeof(MESSAGE_STRUCT));
    char *line = (char *) malloc(500);

    OPENLOG_EnterAppendFileMode(&openlog_dev, "DATA_LOG.txt");
    int task_counter = 0;

    while (1)
    {
        xQueueReceive(device_queue, buffer, 30);
        message_reciever = (MESSAGE_STRUCT *)buffer;

        if (message_reciever->id == 'r')
        {
            DEADONRTC * rtc = (DEADONRTC *)message_reciever->device;
            Print_DateTime(rtc);
            rtc_dev.hours   = rtc->hours;
            rtc_dev.hours   = rtc->hours;
            rtc_dev.minutes = rtc->minutes;
            rtc_dev.seconds = rtc->seconds;
            rtc_dev.date    = rtc->date;
            rtc_dev.month   = rtc->month;
            rtc_dev.year    = rtc->year;
            message_reciever->id = ' ';
            task_counter++;

        }
        else if (message_reciever->id == 't')
        {
            TMP102_STRUCT *tmp = (TMP102_STRUCT*) message_reciever->device;
            float temperature = TMP102_Get_Temperature(tmp);
            printf("Temperature = %f C\n", temperature);
            temperature = TMP102_Get_TemperatureF(tmp);
            printf("Temperature = %f F\n", temperature);
            tmp102_dev.temperature = tmp->temperature;
            message_reciever->id = ' ';
            task_counter++;
        }
        if (task_counter >= 2)
        {
            uint8_t hours   = rtc_dev.hours;
            uint8_t minutes = rtc_dev.minutes;
            uint8_t seconds = rtc_dev.seconds;
            uint8_t date    = rtc_dev.date;
            uint8_t month   = rtc_dev.month;
            uint8_t year    = rtc_dev.year;
            float tempc     = TMP102_Get_Temperature(&tmp102_dev);
            float tempf     = TMP102_Get_TemperatureF(&tmp102_dev);

            sprintf(line, "%02d:%02d:%02d, %02d-%02d-%04d, %fC, %fF",hours, minutes, seconds, month, date, year+2000, tempc, tempf);            
            OPENLOG_WriteLineToFile(&openlog_dev, line);
        }
    }

}


void rtc_intr_task(void *pvParameter)
{
    printf("DEADON RTC Task Start!\n");
    DEADONRTC rtc;
    char msg;
    COMMAND_MESSAGE_STRUCT cmd_msg;
    uint8_t code[] = {0x12, 0xF3, 0xBF, 0x65, 0x89, 0x90};
    uint8_t data[6] = {0};
    MESSAGE_STRUCT device_message;
    DEADON_RTC_Begin(&rtc);
    
    DEADON_RTC_SRAM_Burst_Read(0x00, data, 6);
    bool power_lost = true;
    for (int i=0; i<6; i++)
    {
        if (data[i] != code[i])
        {
            power_lost = true;
            break;
        }
        else
        {
            power_lost = false;
        }
    }

    if (power_lost)
    {
        printf("Lost Power!\n");
        DEADON_RTC_SRAM_Burst_Write(0x00, code, 6);
        DEADON_RTC_WRITE_BUILD_DATETIME();
    }
    else 
    {
        printf("Power Not Lost\n");
    }

    DEADON_RTC_ISR_Init(&rtc);
    delay(1000);
    DEADON_RTC_WRITE_ALARM1(10, 0, 0, 0, ALARM1_SECONDS_MATCH);
    DEADON_RTC_WRITE_ALARM2(0,0,0,ALARM2_PER_MIN);
    delay(100);
    DEADON_RTC_Enable_Interrupt(&rtc, true);
    delay(100);
    DEADON_RTC_Enable_Alarms(&rtc, true, true);

    DEADON_RTC_READ_ALARM1_FLAG(&rtc);
    DEADON_RTC_READ_ALARM2_FLAG(&rtc);

    while (1)
    {
        xQueueReceive(queue, &msg, 10);
        xQueueReceive(rtc_command_queue, (COMMAND_MESSAGE_STRUCT*)&cmd_msg, 30);
        if (msg == 'r')
        {
            bool alarm1_flag = DEADON_RTC_READ_ALARM1_FLAG(&rtc);
            bool alarm2_flag = DEADON_RTC_READ_ALARM2_FLAG(&rtc);

            if (alarm1_flag)
            {
                printf("ALARM1 Triggered\n");
                DEADON_RTC_READ_DATETIME(&rtc);
                Print_DateTime(&rtc);
            }
            if (alarm2_flag)
            {
                printf("ALARM2 Triggered\n");
                DEADON_RTC_READ_DATETIME(&rtc);
                char tmp_ready = 'r';
                xQueueSend(alarm_queue, (void *)&tmp_ready, 30);
                device_message.id = 'r';
                device_message.device = (void*)&rtc;
                xQueueSend(device_queue, &device_message, 30);
            }   
            msg = ' ';
        }
        // Evaulate Console Commands
        if (cmd_msg.id == 'p')
        {
            DEADON_RTC_READ_DATETIME(&rtc);
            Print_DateTime(&rtc);
        }
        if (cmd_msg.id == 's')
        {
            DEADON_RTC_WRITE_SECONDS(cmd_msg.arg1);
        }
        if (cmd_msg.id == 'm')
        {
            DEADON_RTC_WRITE_MINUTES(cmd_msg.arg1);
        }
        if (cmd_msg.id == 'h')
        {
            DEADON_RTC_WRITE_12HOURS(cmd_msg.arg1, cmd_msg.arg2);
        }
        if (cmd_msg.id == 't')
        {
            DEADON_RTC_WRITE_24HOURS(cmd_msg.arg1);
        }
        cmd_msg.id = ' ';
    }
}

void tmp102_sleep_task(void *pvParameter)
{
    printf("Initialize Device\n");
    TMP102_STRUCT tmp102_device;
    char msg;
    MESSAGE_STRUCT device_message;
    TMP102_Begin(&tmp102_device);

    TMP102_Set_Conversion_Rate(&tmp102_device, CONVERSION_MODE_1);
    delay(100);
    TMP102_Sleep(&tmp102_device, true);
    delay(300);

    bool oneshot = false;
    
    while (1)
    {
        xQueueReceive(alarm_queue, &msg, 30);
        if (msg == 'r')
        {
            if (oneshot == false)
            {
                TMP102_Set_OneShot(&tmp102_device);
                oneshot = true;
            }
            delay(100);
            bool tmponeshot = TMP102_Get_OneShot(&tmp102_device);

            if (tmponeshot)
            {
                TMP102_Read_Temperature(&tmp102_device);
                printf("Temperature has been Read\n");
                device_message.id = 't';
                device_message.device = (void *)&tmp102_device;
                xQueueSend(device_queue, &device_message, 30);
                oneshot = false;
            }
            msg = ' ';
        }


    }
}


void console_task(void *pvParameter)
{
    Start_Console();

    Register_Console_Commands();

    const char* prompt = LOG_COLOR_I "esp> " LOG_RESET_COLOR;

    printf("\n"
        "This is an example of ESP-IDF console component.\n"
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
        char* line = linenoise(prompt);
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


void app_main()
{
    printf("Starting Tasks!\n");
    device_queue = xQueueCreate(3, sizeof(MESSAGE_STRUCT));
    alarm_queue = xQueueCreate(3, 1);
    register_queues();

    xTaskCreate(&rtc_intr_task, "rtc_intr_task", configMINIMAL_STACK_SIZE*3, NULL, 5, NULL);
    xTaskCreate(&tmp102_sleep_task, "tmp102sleep_task", configMINIMAL_STACK_SIZE*4, NULL, 6, NULL);
    //xTaskCreate(&openlog_task, "openlog_task", configMINIMAL_STACK_SIZE*4, NULL, 7, NULL);
    xTaskCreate(&openlog_task_dummy, "openlog_task_dummy", configMINIMAL_STACK_SIZE*4, NULL, 7, NULL);

    xTaskCreate(&console_task, "console_task", configMINIMAL_STACK_SIZE*4, NULL, 7, NULL);

}
