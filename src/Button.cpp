#include "Button.h"
#include "esp_err.h"

Button::Button(gpio_num_t pin)
    : pin(pin)
{
    gpio_set_direction(pin, GPIO_MODE_INPUT);
    gpio_set_pull_mode(pin, GPIO_PULLDOWN_ONLY);
}

Button::operator bool() const
{
    return (bool)gpio_get_level(pin);
}

ButtonInterrupt::ButtonInterrupt(gpio_num_t pin, gpio_isr_t isr_handler)
    : pin(pin)
{

    gpio_config_t io_conf;
    io_conf.intr_type = (gpio_int_type_t)GPIO_PIN_INTR_NEGEDGE;
    io_conf.pin_bit_mask = 1ULL << pin;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    esp_err_t err = gpio_config(&io_conf);
    ESP_ERROR_CHECK(err);
    err = gpio_set_intr_type(pin, GPIO_INTR_NEGEDGE);
    ESP_ERROR_CHECK(err);
    err = gpio_install_isr_service(0);
    ESP_ERROR_CHECK(err);
    err = gpio_isr_handler_add(pin, isr_handler, (void *)NULL);
    ESP_ERROR_CHECK(err);
}
