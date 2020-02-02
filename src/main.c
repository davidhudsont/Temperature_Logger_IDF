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


#define BLINK_GPIO (5)

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

void blink_task(void *pvParameter)
{
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    printf("Blink Task Start!\n");

    gpio_pad_select_gpio(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    while(1) {
        /* Blink off (output low) */
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("Blink Off!\n");
        /* Blink on (output high) */
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("Blink On!\n");
        fflush(stdout);
    }
}

void rtc_task(void *pvParameter)
{
    printf("DEADON RTC Task Start!\n");
    DEADONRTC rtc;
    memset(&rtc, 0, sizeof(DEADONRTC));

    volatile int clock_speed = 4*1000*1000; // Clock speed 4MHz

    esp_err_t err = false;
    err = BSP_SPI_Init(clock_speed);
    if (err != ESP_OK)
    {
        printf("Spi Configuration Error\n");
    }
    
    //DEADON_RTC_WRITE_DATETIME(0, 33, 16, 7, 19, 1, 20);
    DEADON_RTC_WRITE_BUILD_DATETIME();
    delay(1000);
    static int last_seconds = -1;

    while (1)
    {

        DEADON_RTC_READ_DATETIME(&rtc);

        if (last_seconds != rtc.raw_time[0])
        {
            /*
            for (int i =0; i<7; i++)
            {
                printf("RAW TIME [%d]: 0x%x\n", i, (unsigned int) rtc.raw_time[i]);
            }
            */
            Print_DateTime(&rtc);
            last_seconds = rtc.raw_time[0];
        }
        delay(100);
    }
}


void rtc_intr_task(void *pvParameter)
{
    printf("DEADON RTC Task Start!\n");
    DEADONRTC rtc;
    char msg;
    DEADON_RTC_Begin(&rtc);
    
    //DEADON_RTC_WRITE_DATETIME(0, 33, 16, 7, 19, 1, 20);
    DEADON_RTC_WRITE_BUILD_DATETIME();
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
            }   
            msg = ' ';
        }
    }
}

void rtc_sram_task(void *pvParameter)
{
    printf("DEADON RTC Task Start!\n");
    DEADONRTC rtc;
    uint8_t data[] = {0x12,0xFE,0xAB,0x35,0xFA,0xE6};

    DEADON_RTC_Begin(&rtc);
    
    //DEADON_RTC_WRITE_DATETIME(0, 33, 16, 7, 19, 1, 20);
    DEADON_RTC_WRITE_BUILD_DATETIME();
    delay(1000);
    DEADON_RTC_SRAM_Burst_Write(0x23, data, 6);
    while (1)
    {
        uint8_t sram_data[6] = {0};
        DEADON_RTC_SRAM_Burst_Read(0x23, sram_data, 6);
        printf("SRAM Data:\n");
        for (int i=0; i<6; i++)
        {
            printf("\tSRAM[0x%x] = 0x%x\n",(unsigned int)(i+0x23),(unsigned int) sram_data[i]);
        }
        delay(1000);

    }
}

void app_main()
{
    printf("Starting Tasks!\n");

    //xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
    //xTaskCreate(&rtc_task, "rtc_task", configMINIMAL_STACK_SIZE*3, NULL, 5, NULL);
    //xTaskCreate(&rtc_intr_task, "rtc_intr_task", configMINIMAL_STACK_SIZE*3, NULL, 5, NULL);
    xTaskCreate(&rtc_sram_task, "rtc_sram_task", configMINIMAL_STACK_SIZE*3, NULL, 5, NULL);


}
