#pragma once

#include "driver/gpio.h"

class Button
{
public:
    Button(gpio_num_t pin);

    operator bool() const;

private:
    gpio_num_t pin;
};

class ButtonInterrupt
{
public:
    ButtonInterrupt(gpio_num_t pin, gpio_isr_t isr_handler);

private:
    gpio_num_t pin;
};