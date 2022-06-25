//
// Created by Braden Nicholson on 6/13/22.
//
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "beam.h"
#include "servo.h"
#include "wifi.h"
#include "indicator.h"
#include "server.h"

// Beam Assignments
#define BEAM_PRIMARY_PIN CONFIG_SENTRY_BEAM_PRIMARY_GPIO
#define BEAM_SECONDARY_PIN CONFIG_SENTRY_BEAM_SECONDARY_GPIO

// Servo Assignments
#define SERVO_PAN CONFIG_SENTRY_PAN_SERVO_GPIO
#define SERVO_TILT CONFIG_SENTRY_TILT_SERVO_GPIO

void app_main(void) {


    initIndicator();
    setIndicator(RED);

    setupServer();

    Servo pan = configureServo(SERVO_PAN, MCPWM_UNIT_0, MCPWM_TIMER_0);
    Servo tilt = configureServo(SERVO_TILT, MCPWM_UNIT_1, MCPWM_TIMER_0);

    Beam primary = configureBeam(BEAM_PRIMARY_PIN, LEDC_CHANNEL_1, LEDC_TIMER_2);
    Beam target = configureBeam(BEAM_SECONDARY_PIN, LEDC_CHANNEL_0, LEDC_TIMER_1);

    uint32_t duty = 0;
    int deg = 0;
    int dir = 1;

    while (1) {

        if (deg >= 90) {
            dir = 0;
        } else if (deg <= -90) {
            dir = 1;
        }

        deg += dir == 1 ? 1 : -1;
        duty = (duty + 100) % 8191;

        setBeamOutput(target, duty);
        setBeamOutput(primary, 8191 - duty);

        applyBeamOutput(target);
        applyBeamOutput(primary);

        moveTo(pan, deg);
        moveTo(tilt, deg);

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
