//
// Created by Braden Nicholson on 6/25/22.
//

#include "sentry.h"
#include <cJSON.h>
#include <esp_mac.h>

char *formatJSON(Sentry sentry) {

    // Parent Status Object
    cJSON *status = cJSON_CreateObject();

    // System Info
    cJSON *system = cJSON_CreateObject();

    // Get the mac address
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    char address[48];
    sprintf(address, "%x:%x:%x:%x:%x:%x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    cJSON_AddItemToObject(system, "mac", cJSON_CreateString(address));

    cJSON_AddItemToObject(status, "system", system);
    // Servo Object
    cJSON *servos = cJSON_CreateObject();

    cJSON *panServo = cJSON_CreateNumber(sentry.pan.position);
    cJSON_AddItemToObject(servos, "pan", panServo);

    cJSON *tiltServo = cJSON_CreateNumber(sentry.tilt.position);
    cJSON_AddItemToObject(servos, "tilt", tiltServo);

    // Beam Object
    cJSON *beams = cJSON_CreateObject();

    cJSON *beamPrimary = cJSON_CreateBool(sentry.primary.active == 1);
    cJSON_AddItemToObject(beams, "primary", beamPrimary);

    cJSON *beamSecondary = cJSON_CreateBool(sentry.secondary.active == 1);
    cJSON_AddItemToObject(beams, "secondary", beamSecondary);

    cJSON_AddItemToObject(status, "servos", servos);
    cJSON_AddItemToObject(status, "beams", beams);

    char *formatted = cJSON_Print(status);

    cJSON_Delete(status);

    return formatted;
}