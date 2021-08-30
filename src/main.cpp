/**
 * @author David Hudson
 * @brief Main Application for Temperature Logger
 * 
 */

#ifdef NATIVE

int main()
{
    return 0;
}

#else

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "System_Tasks.h"
#include "esp_log.h"

extern "C"
{
    void app_main(void);
}

// cppcheck-suppress unusedFunction
void app_main()
{
    ESP_LOGI("Main", "Starting Tasks");
    Create_Semaphores();
    Create_Tasks();
}

#endif