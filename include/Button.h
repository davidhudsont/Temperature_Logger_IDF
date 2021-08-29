#pragma once

#include "driver/gpio.h"
#include "esp_timer.h"

class Button
{
public:
    Button(gpio_num_t pin);

    operator bool();

private:
    bool get_button_state();
    gpio_num_t pin;
    int64_t lastDebounceTime;
    bool buttonState;
    bool lastButtonState;
    const int debounceDelay = 50; // the debounce time(ms); increase if the output flickers
};

class ButtonInterrupt
{
public:
    ButtonInterrupt(gpio_num_t pin, gpio_isr_t isr_handler);

private:
    gpio_num_t pin;
};