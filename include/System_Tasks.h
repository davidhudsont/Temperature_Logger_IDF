
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

#ifdef __cplusplus
extern "C"
{
#endif

    void Create_Task_Queues();
    void Create_Tasks();

#ifdef __cplusplus
}
#endif

#endif