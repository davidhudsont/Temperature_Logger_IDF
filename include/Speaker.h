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

public:
    AlarmSpeaker();
    AlarmSpeaker(gpio_num_t pin);
    void SetPWM(uint32_t duty);
    void PauseSound();
    void PlaySound();
    void StartAlarm();
    void StopAlarm();
    void ProcessAlarm();
};
