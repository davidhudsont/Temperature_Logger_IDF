/**
 * @file ConsoleCommands.h
 * @author David Hudson
 * @brief Header file
 * @date 2020-02-21
 * 
 */

#ifndef __REGISTER_COMMANDS_H_
#define __REGISTER_COMMANDS_H_

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_console.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "argtable3/argtable3.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"


QueueHandle_t rtc_command_queue; // Queue to send device objects between tasks
QueueHandle_t tmp_command_queue; // Queue to send device objects between tasks
QueueHandle_t openlog_command_queue; // Queue to send device objects between tasks


typedef enum COMMAND_ID {
    COMMAND_NULL = -1,
    COMMAND_SET_SECONDS = 0,
    COMMAND_SET_MINUTES,
    COMMAND_SET_12HOURS,
    COMMAND_SET_24HOURS,
    COMMAND_SET_WEEKDAY,
    COMMAND_SET_DATE,
    COMMAND_SET_MONTH,
    COMMAND_SET_YEAR,
    COMMAND_GET_DATETIME,
    COMMAND_GET_TEMPF,
    COMMAND_GET_TEMPC,
    COMMAND_STOP_LOG,
} COMMAND_ID;

typedef struct COMMAND_MESSAGE_STRUCT {
    COMMAND_ID id;
    int  arg1;
    int  arg2;
    int  arg3;
} COMMAND_MESSAGE_STRUCT;

void register_system(void);
void register_queues(void);


#endif