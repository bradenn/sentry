//
// Created by Braden Nicholson on 6/13/22.
//
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "beam.h"
#include "servo.h"

void app_main(void) {
    /* Configure the peripheral according to the LED type */


    Beam target = configureBeam(25, LEDC_CHANNEL_0, LEDC_TIMER_0);
    Beam primary = configureBeam(26, LEDC_CHANNEL_1, LEDC_TIMER_1);
    Servo servo1 = configureServo(14, LEDC_CHANNEL_2, LEDC_TIMER_2);

    uint32_t duty = 0;
    while (1) {
        duty = (duty + 1) % 8191;
        setBeamOutput(target, duty);
        setBeamOutput(primary, 8191 - duty);
        moveTo(servo1, 180);
        applyBeamOutput(target);
        applyBeamOutput(primary);
        /* Toggle the LED state */
        vTaskDelay(7 / portTICK_PERIOD_MS);
    }
}
