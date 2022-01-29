#include "Speaker.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include <string.h>

constexpr ledc_timer_t LEDC_TIMER = LEDC_TIMER_0;
constexpr ledc_mode_t LEDC_MODE = LEDC_LOW_SPEED_MODE;
constexpr gpio_num_t LEDC_OUTPUT_IO = GPIO_NUM_26; // Define the output GPIO
constexpr ledc_channel_t LEDC_CHANNEL = LEDC_CHANNEL_0;
constexpr ledc_timer_bit_t LEDC_DUTY_RES = LEDC_TIMER_13_BIT; // Set duty resolution to 13 bits
constexpr int LEDC_DUTY = 2047;                               // Set duty to 50%. ((2 ** 13) - 1) * 50% = 4095
constexpr int LEDC_FREQUENCY = 5000;                          // Frequency in Hertz. Set frequency at 5 kHz

constexpr int MAXDUTYCYCLE = 8191;

static SemaphoreHandle_t speaker_semaphore;

static void periodic_cb(void *param)
{
    xSemaphoreGive(speaker_semaphore);
}

AlarmSpeaker::AlarmSpeaker()
    : is_timer_started(false),
      pin(GPIO_NUM_26)
{
}

AlarmSpeaker::AlarmSpeaker(gpio_num_t pin)
    : is_timer_started(false),
      pin(pin)
{
}

void AlarmSpeaker::Init()
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer;
    memset(&ledc_timer, 0, sizeof(ledc_timer_config_t));
    ledc_timer.speed_mode = LEDC_MODE;
    ledc_timer.timer_num = LEDC_TIMER;
    ledc_timer.duty_resolution = LEDC_DUTY_RES;
    ledc_timer.freq_hz = LEDC_FREQUENCY; // Set output frequency at 5 kHz
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel;
    memset(&ledc_channel, 0, sizeof(ledc_channel_config_t));
    ledc_channel.speed_mode = LEDC_MODE;
    ledc_channel.channel = LEDC_CHANNEL;
    ledc_channel.timer_sel = LEDC_TIMER;
    ledc_channel.intr_type = LEDC_INTR_DISABLE;
    ledc_channel.gpio_num = pin;
    ledc_channel.duty = 0; // Set duty to 0%
    ledc_channel.hpoint = 0;
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    speaker_semaphore = xSemaphoreCreateBinary();

    const esp_timer_create_args_t timer_config = {
        .callback = &periodic_cb,
        .arg = (void *)0,
        .dispatch_method = (esp_timer_dispatch_t)0,
        .name = "Periodic",
    };

    ESP_ERROR_CHECK(esp_timer_create(&timer_config, &periodic_timer));
}

void AlarmSpeaker::SetPWM(uint32_t duty)
{
    // Clamp to the MAXDUTYCYCLE
    if (duty > MAXDUTYCYCLE)
        duty = MAXDUTYCYCLE;
    // Set duty cycle
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty));
    // Update duty to apply the new value
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
}

void AlarmSpeaker::PauseSound()
{
    ledc_timer_pause(LEDC_MODE, LEDC_TIMER);
}

void AlarmSpeaker::PlaySound()
{
    ledc_timer_resume(LEDC_MODE, LEDC_TIMER);
}

void AlarmSpeaker::StartAlarm()
{
    if (!is_timer_started)
    {
        ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 1000000));
        is_timer_started = true;
    }
}

void AlarmSpeaker::StopAlarm()
{
    if (is_timer_started)
    {
        ESP_ERROR_CHECK(esp_timer_stop(periodic_timer));
        is_timer_started = false;
    }
}

void AlarmSpeaker::ProcessAlarm()
{
    static int sound_on = false;
    if (xSemaphoreTake(speaker_semaphore, 10))
    {
        if (sound_on)
        {
            PauseSound();
            sound_on = false;
        }
        else
        {
            PlaySound();
            sound_on = true;
        }
    }
}

void AlarmSpeaker::SetFrequency(uint32_t freq_hz)
{
    if (freq_hz <= LEDC_FREQUENCY)
        ESP_ERROR_CHECK(ledc_set_freq(LEDC_MODE, LEDC_TIMER, freq_hz));
}
