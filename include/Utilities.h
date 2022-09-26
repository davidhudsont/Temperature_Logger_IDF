#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/**
 * @brief Delays a task for the passed
 *        in parameter time_ms in milliseconds
 * @param time_ms
 */
void delay_ms(uint32_t time_ms)
{
    vTaskDelay(time_ms / portTICK_PERIOD_MS);
}
