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

void app_main(void) {

    initIndicator();

    int result = initWifi();

    setIndicator(result);

    Beam target = configureBeam(25, LEDC_CHANNEL_0, LEDC_TIMER_0);
    Beam primary = configureBeam(26, LEDC_CHANNEL_1, LEDC_TIMER_1);

    uint32_t duty = 0;

    while (1) {
        duty = (duty + 10) % 8191;
        setBeamOutput(target, duty);
        setBeamOutput(primary, 8191 - duty);

        applyBeamOutput(target);
        applyBeamOutput(primary);

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
