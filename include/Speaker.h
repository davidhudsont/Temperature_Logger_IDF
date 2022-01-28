#pragma once

#include <stdio.h>
#include "driver/gpio.h"

class Speaker
{
private:
    /* data */
public:
    Speaker();
    Speaker(gpio_num_t pin);
    void SetPWM(uint32_t duty);
    void PauseSound();
    void PlaySound();
};
