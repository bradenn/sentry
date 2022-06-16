//
// Created by Braden Nicholson on 6/14/22.
//

#include "beam.h"

#include <driver/gpio.h>
#include <driver/ledc.h>


Beam configureBeam(gpio_num_t gpio, ledc_channel_t channel, ledc_timer_t timer) {
    // Enable the PWM timer
    ledc_timer_config_t ledc_timer = {
            .speed_mode       = BEAM_SPEED_MODE,
            .timer_num        = timer,
            .duty_resolution  = LEDC_TIMER_13_BIT,
            .freq_hz          = 5000,  // 2.5khz
            .clk_cfg          = LEDC_AUTO_CLK
    };

    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Enable the channel
    ledc_channel_config_t ledc_channel = {
            .speed_mode     = BEAM_SPEED_MODE,
            .channel        = channel,
            .timer_sel      = timer,
            .intr_type      = LEDC_INTR_DISABLE,
            .gpio_num       = gpio,
            .duty           = 0, // Set duty to 0%
            .hpoint         = 0
    };

    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    Beam target;
    target.gpio = gpio;
    target.channel = channel;

    return target;
}

int map_range(int value, int low1, int high1, int low2, int high2) {
return low2 + (high2 - low2) * (value - low1) / (high1 - low1);
}

void setBeamOpticalOutput(Beam target, int mw) {
    int mapped = map_range(mw, 0, target.opticalOutput, 0, (1 << 13)-1);
    ESP_ERROR_CHECK(ledc_set_duty(BEAM_SPEED_MODE, target.channel, mapped));

}

void setBeamOutput(Beam target, uint32_t duty) {

    ESP_ERROR_CHECK(ledc_set_duty(BEAM_SPEED_MODE, target.channel, duty));

}

void applyBeamOutput(Beam target) {

    ESP_ERROR_CHECK(ledc_update_duty(BEAM_SPEED_MODE, target.channel));

}
