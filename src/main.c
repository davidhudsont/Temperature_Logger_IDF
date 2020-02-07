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

    printf("%02d:%02d:%02d, %02d-%02d-%04d\n",hours, minutes, seconds, month, date, year+2000);

}


void openlog_task(void *pvParameter)
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


void rtc_intr_task(void *pvParameter)
{
    printf("DEADON RTC Task Start!\n");
    DEADONRTC rtc;
    char msg;
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



void app_main()
{
    printf("Starting Tasks!\n");
    device_queue = xQueueCreate(3, sizeof(MESSAGE_STRUCT));
    alarm_queue = xQueueCreate(3, 1);

    xTaskCreate(&rtc_intr_task, "rtc_intr_task", configMINIMAL_STACK_SIZE*3, NULL, 5, NULL);
    xTaskCreate(&tmp102_sleep_task, "tmp102sleep_task", configMINIMAL_STACK_SIZE*4, NULL, 6, NULL);
    xTaskCreate(&openlog_task, "openlog_task", configMINIMAL_STACK_SIZE*4, NULL, 7, NULL);


}
