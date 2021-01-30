
#ifndef _BSP_CONSOLE_H_
#define _BSP_CONSOLE_H_

#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"
#include "esp_vfs_fat.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "ConsoleCommands.h"

void Start_Console();
void Register_Console_Commands();

#endif