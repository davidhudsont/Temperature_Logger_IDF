#pragma once

#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_timer.h"

#define PIN_NUM_SPEAKER (GPIO_NUM_26)

class AlarmSpeaker
{
public:
    AlarmSpeaker();
    void StartAlarm();
    void StopAlarm();
    void SetFrequency(uint32_t freq_hz);
    void SetDutyCycle(uint32_t duty_cycle);
    void SetDutyCyclePercentage(uint32_t duty_cycle_percentage);

private:
    bool is_timer_started;
    esp_timer_handle_t periodic_timer;
    gpio_num_t pin;
};
