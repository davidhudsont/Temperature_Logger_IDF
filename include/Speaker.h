#pragma once

#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_timer.h"

class AlarmSpeaker
{
private:
    /* data */
    bool is_timer_started;
    esp_timer_handle_t periodic_timer;
    gpio_num_t pin;

public:
    AlarmSpeaker();
    AlarmSpeaker(gpio_num_t pin);
    void Init();
    void StartAlarm();
    void StopAlarm();
    void SetFrequency(uint32_t freq_hz);
    void SetDutyCycle(uint32_t duty_cycle);
    void SetDutyCyclePercentage(uint32_t duty_cycle_percentage);
};
