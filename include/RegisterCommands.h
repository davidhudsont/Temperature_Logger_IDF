
#ifndef __REGISTER_COMMANDS_H_
#define __REGISTER_COMMANDS_H_

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "esp_log.h"
#include "esp_console.h"
#include "esp_system.h"
#include "esp_sleep.h"
#include "esp_spi_flash.h"
#include "driver/rtc_io.h"
#include "driver/uart.h"
#include "argtable3/argtable3.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "sdkconfig.h"


QueueHandle_t blink_queue; // Queue to send device objects between tasks
QueueHandle_t rtc_command_queue; // Queue to send device objects between tasks
QueueHandle_t tmp_command_queue; // Queue to send device objects between tasks
QueueHandle_t openlog_command_queue; // Queue to send device objects between tasks


typedef struct COMMAND_MESSAGE_STRUCT {
    char id;
    int  arg1;
    int  arg2;
    int  arg3;
} COMMAND_MESSAGE_STRUCT;

void register_system(void);
void register_queues(void);


#endif