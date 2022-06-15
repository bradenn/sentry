#include <driver/gpio.h>
#include <driver/ledc.h>

#include "servo.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

//
// Created by Braden Nicholson on 6/14/22.
//
Servo configureServo(gpio_num_t gpio, ledc_channel_t channel, ledc_timer_t timer) {
    // Enable the PWM timer
    ledc_timer_config_t ledc_timer = {
            .speed_mode       = LEDC_HIGH_SPEED_MODE,
            .timer_num        = timer,
            .duty_resolution  = LEDC_TIMER_13_BIT,
            .freq_hz          = 50,  // 50hz
            .clk_cfg          = LEDC_AUTO_CLK
    };

    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Enable the channel
    ledc_channel_config_t ledc_channel = {
            .speed_mode     = LEDC_HIGH_SPEED_MODE,
            .channel        = channel,
            .timer_sel      = timer,
            .intr_type      = LEDC_INTR_DISABLE,
            .gpio_num       = gpio,
            .duty           = 0, // Set duty to 0%
            .hpoint         = 0
    };

    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    Servo target;
    target.gpio = gpio;
    target.channel = channel;

    return target;
}

void moveTo(Servo servo, int position) {
    // Write this
}