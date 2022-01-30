#include "Button.h"
#include "esp_err.h"

static int64_t millis()
{
    return esp_timer_get_time() / 1000;
}

Button::Button(gpio_num_t pin)
    : pin(pin),
      lastDebounceTime(0),
      buttonState(false),
      lastButtonState(false)
{
    gpio_set_direction(pin, GPIO_MODE_INPUT);
    gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);
}

bool Button::Debounce()
{
    bool ret = false;
    int reading = !gpio_get_level(pin);
    // If the switch changed, due to noise or pressing:
    if (reading != lastButtonState)
    {
        // reset the debouncing timer
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay)
    {
        // whatever the reading is at, it's been there for longer than the debounce
        // delay, so take it as the actual current state:

        // if the button state has changed:
        if (reading != buttonState)
        {
            buttonState = reading;

            // only toggle the LED if the new button state is HIGH
            if (buttonState)
            {
                ret = true;
            }
        }
    }

    // save the reading. Next time through the loop, it'll be the lastButtonState:
    lastButtonState = reading;

    return ret;
}

Button::operator bool()
{
    return Debounce();
}

ButtonInterrupt::ButtonInterrupt(gpio_num_t pin, gpio_isr_t isr_handler)
    : pin(pin)
{

    gpio_config_t io_conf;
    io_conf.intr_type = (gpio_int_type_t)GPIO_PIN_INTR_POSEDGE;
    io_conf.pin_bit_mask = 1ULL << pin;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    esp_err_t err = gpio_config(&io_conf);
    ESP_ERROR_CHECK(err);
    err = gpio_set_intr_type(pin, GPIO_INTR_NEGEDGE);
    ESP_ERROR_CHECK(err);
    err = gpio_isr_handler_add(pin, isr_handler, (void *)NULL);
    ESP_ERROR_CHECK(err);
}
