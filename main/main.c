//
// Created by Braden Nicholson on 6/13/22.
//

#include <math.h>

#include "sdkconfig.h"

#include "beam.h"
#include "servo.h"
#include "indicator.h"
#include "server.h"

// Beam Assignments
#define BEAM_PRIMARY_PIN CONFIG_SENTRY_BEAM_PRIMARY_GPIO
#define BEAM_SECONDARY_PIN CONFIG_SENTRY_BEAM_SECONDARY_GPIO

// Servo Assignments
#define SERVO_PAN CONFIG_SENTRY_PAN_SERVO_GPIO
#define SERVO_TILT CONFIG_SENTRY_TILT_SERVO_GPIO



void app_main(void) {



    static Sentry sentry;

    init(&sentry);

    sentry.pan = configureServo(SERVO_PAN, MCPWM_UNIT_0, MCPWM_TIMER_0);
    sentry.tilt = configureServo(SERVO_TILT, MCPWM_UNIT_1, MCPWM_TIMER_0);

    moveTo(&sentry.pan, 0);
    moveTo(&sentry.tilt, 0);
    initIndicator();
    setIndicator(RED);
    sentry.primary = configureBeam(BEAM_PRIMARY_PIN, LEDC_CHANNEL_1, LEDC_TIMER_2);
    sentry.secondary = configureBeam(BEAM_SECONDARY_PIN, LEDC_CHANNEL_0, LEDC_TIMER_2);



    setupServer(sentry);
}
