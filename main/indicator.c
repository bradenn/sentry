//
// Created by Braden Nicholson on 6/14/22.
//


#include <driver/gpio.h>
#include "indicator.h"

#define INDICATOR_RED CONFIG_GPIO_INDICATOR_RED
#define INDICATOR_GREEN CONFIG_GPIO_INDICATOR_GREEN
#define INDICATOR_BLUE CONFIG_GPIO_INDICATOR_BLUE

void assignGPIOPin(int gpio) {
    // Reset any preset values for the target gpio pin
    gpio_reset_pin(gpio);
    // Set the pin as an output
    gpio_set_direction(gpio, GPIO_MODE_OUTPUT);
    // Set the value to zero
    gpio_set_level(gpio, 0);
}


void setIndicator(indicatorColor color) {
    switch (color) {
        case RED:
            gpio_set_level(INDICATOR_RED, 1);
            gpio_set_level(INDICATOR_GREEN, 0);
            gpio_set_level(INDICATOR_BLUE, 0);
            break;
        case GREEN:
            gpio_set_level(INDICATOR_RED, 0);
            gpio_set_level(INDICATOR_GREEN, 1);
            gpio_set_level(INDICATOR_BLUE, 0);
            break;
        case BLUE:
            gpio_set_level(INDICATOR_RED, 0);
            gpio_set_level(INDICATOR_GREEN, 0);
            gpio_set_level(INDICATOR_BLUE, 1);
            break;
        default:
            break;
    }
}

void initIndicator() {

    assignGPIOPin(INDICATOR_RED);
    assignGPIOPin(INDICATOR_BLUE);
    assignGPIOPin(INDICATOR_GREEN);

}