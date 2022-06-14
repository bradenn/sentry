//
// Created by Braden Nicholson on 6/14/22.
//
#ifndef MAIN_SERVO_H
#define MAIN_SERVO_H
#include <driver/gpio.h>

typedef struct Servo {
    gpio_num_t gpio;
    ledc_channel_t channel;
} Servo;

Servo configureServo(gpio_num_t gpio, ledc_channel_t channel, ledc_timer_t timer);

void moveTo(Servo servo, int position);

#endif //MAIN_SERVO_H
