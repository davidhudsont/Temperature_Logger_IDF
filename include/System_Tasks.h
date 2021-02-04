
#ifndef __SYSTEM_TASKS_H_
#define __SYSTEM_TASKS_H_

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "bspSpi.h"
#include "DEADONRTC.h"
#include "TMP102.h"
#include "OPENLOG.h"
#include "bspConsole.h"

// Used to communicate between tasks
typedef struct MESSAGE_STRUCT
{
    char id;
    void *device;
} MESSAGE_STRUCT;

QueueHandle_t device_queue; // Queue to send device objects between tasks

QueueHandle_t alarm_queue; // Sends message when an alarm has been triggered

#ifdef __cplusplus
  extern "C" {
#endif

void Create_Task_Queues();
void Create_Tasks();

#ifdef __cplusplus
  }
#endif

#endif