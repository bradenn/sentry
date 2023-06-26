//
// Created by Braden Nicholson on 6/25/22.
//

#ifndef SENTRY_SENTRY_H
#define SENTRY_SENTRY_H

#include <driver/temperature_sensor.h>
#include "servo.h"
#include "beam.h"
#include <esp_http_server.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"

typedef struct Request {
    int pan;
    int tilt;
} Request;

typedef struct Sentry {
    Servo pan;
    Servo tilt;


    RingbufHandle_t fifo;

    int targetPan;
    int targetTilt;
    SemaphoreHandle_t mutex;
    Beam primary;
    Beam secondary;


    httpd_handle_t hd;
    int fd;

    char macAddress[48];

} Sentry;

Request lastRequest(Sentry *s);

void sendUpdateToClient(Sentry *s);

void setSession(Sentry *sentry, httpd_req_t *ses);

void moveSentry(Sentry *sentry, int pan, int tilt);

void init(Sentry *);

char *formatJSON(Sentry *);


#endif //SENTRY_SENTRY_H
