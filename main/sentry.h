//
// Created by Braden Nicholson on 6/25/22.
//

#ifndef SENTRY_SENTRY_H
#define SENTRY_SENTRY_H

#include "servo.h"
#include "beam.h"

typedef struct Sentry {
    Servo pan;
    Servo tilt;

    Beam primary;
    Beam secondary;
} Sentry;

char *formatJSON(Sentry);


#endif //SENTRY_SENTRY_H
