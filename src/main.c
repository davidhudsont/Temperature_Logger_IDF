/**
 * @file main.c
 * @author David Hudson
 * @brief 
 * @version 0.2
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

#define BLINK_GPIO (5)

QueueHandle_t alarm_queue;

void delay(uint32_t time_ms)
{
    vTaskDelay( time_ms / portTICK_PERIOD_MS);
}

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


void rtc_intr_task(void *pvParameter)
{
    printf("DEADON RTC Task Start!\n");
    DEADONRTC rtc;
    char msg;
    uint8_t code[] = {0x12, 0xF3, 0xBF, 0x65, 0x89, 0x90};
    uint8_t data[6] = {0};
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
                printf("ALARM1:\n\t");
                DEADON_RTC_READ_DATETIME(&rtc);
                Print_DateTime(&rtc);
            }
            if (alarm2_flag)
            {
                printf("ALARM2:\n\t");
                DEADON_RTC_READ_DATETIME(&rtc);
                Print_DateTime(&rtc);
                char tmp_ready = 'r';
                xQueueSend(alarm_queue, (void *)&tmp_ready, 30);
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
                float temperature = TMP102_Get_Temperature(&tmp102_device);
                printf("Temperature = %f C\n", temperature);
                temperature = TMP102_Get_TemperatureF(&tmp102_device);
                printf("Temperature = %f F\n", temperature);
                oneshot = false;
            }
            msg = ' ';
        }


    }
}



void app_main()
{
    printf("Starting Tasks!\n");
    alarm_queue = xQueueCreate(3, 1);

    xTaskCreate(&rtc_intr_task, "rtc_intr_task", configMINIMAL_STACK_SIZE*3, NULL, 5, NULL);
    xTaskCreate(&tmp102_sleep_task, "tmp102sleep_task", configMINIMAL_STACK_SIZE*4, NULL, 6, NULL);


}
