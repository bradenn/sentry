//
// Created by Braden Nicholson on 6/25/22.
//

#include "sentry.h"
#include <cJSON.h>
#include <esp_mac.h>


//double readTemp(Sentry sentry) {
//    ESP_ERROR_CHECK(temperature_sensor_enable(sentry.temp));
//    float tsens_out;
//    ESP_ERROR_CHECK(temperature_sensor_get_celsius(sentry.temp, &tsens_out));
//    ESP_ERROR_CHECK(temperature_sensor_disable(sentry.temp));
//    return (double) tsens_out;
//}
static TaskHandle_t handle;
char *formatJSON(Sentry *sentry) {

    // Parent Status Object
    cJSON *status = cJSON_CreateObject();

    // System Info
    cJSON *system = cJSON_CreateObject();

    // Get the mac address

    cJSON_AddItemToObject(system, "mac", cJSON_CreateString(sentry->macAddress));
//    cJSON_AddItemToObject(system, "temp", cJSON_CreateNumber(readTemp(sentry)));

    cJSON_AddItemToObject(status, "system", system);
    // Servo Object
    cJSON *servos = cJSON_CreateObject();

    cJSON *panServo = cJSON_CreateNumber(sentry->targetPan);
    cJSON_AddItemToObject(servos, "pan", panServo);

    cJSON *tiltServo = cJSON_CreateNumber(sentry->targetTilt);
    cJSON_AddItemToObject(servos, "tilt", tiltServo);

    // Beam Object
    cJSON *beams = cJSON_CreateObject();

    cJSON *primaryBeam = cJSON_CreateObject();
    cJSON_AddItemToObject(primaryBeam, "active", cJSON_CreateBool(sentry->primary.active == 1));
    cJSON_AddItemToObject(primaryBeam, "power", cJSON_CreateNumber(sentry->primary.duty * (double) sentry->primary
            .opticalOutput));

    cJSON *secondaryBeam = cJSON_CreateObject();
    cJSON_AddItemToObject(secondaryBeam, "active", cJSON_CreateBool(sentry->secondary.active == 1));
    cJSON_AddItemToObject(secondaryBeam, "power", cJSON_CreateNumber(sentry->secondary.duty * (double) sentry->secondary
            .opticalOutput));

    cJSON_AddItemToObject(beams, "primary", primaryBeam);
    cJSON_AddItemToObject(beams, "secondary", secondaryBeam);

    cJSON_AddItemToObject(status, "servos", servos);
    cJSON_AddItemToObject(status, "beams", beams);

    char *formatted = cJSON_PrintUnformatted(status);

    cJSON_Delete(status);

    return formatted;
}

void setSession(Sentry *s, httpd_req_t *ses) {
    s->hd = ses->handle;
    s->fd = httpd_req_to_sockfd(ses);
}

void sendUpdateToClient(Sentry *s) {
    if(s->fd < 0) {
        return;
    }
    httpd_ws_frame_t out_packer;
    memset(&out_packer, 0, sizeof(httpd_ws_frame_t));

    char *resp_str = formatJSON(s);

    out_packer.payload = (uint8_t *) resp_str;
    out_packer.len = strlen(resp_str);
    out_packer.type = HTTPD_WS_TYPE_TEXT;

    esp_err_t ret = httpd_ws_send_frame_async(s->hd, s->fd, &out_packer);
    free(resp_str);
    if (ret != ESP_OK) {
        printf("Send to bridge failed!\n");
        s->fd = -1;
        httpd_sess_trigger_close(s->hd, s->fd);
        return;
    }
}



void moveSentry(Sentry *sentry, int pan, int tilt) {
    Request r = {
            .tilt = tilt,
            .pan = pan,
    };
    sentry->targetPan = pan;
    sentry->targetTilt = tilt;
    if (xRingbufferSend(sentry->fifo, (void *) &r, sizeof(r), pdMS_TO_TICKS(10)) != pdTRUE) {
        printf("Overflow :< \n");
    }
}

static int changedA = 0;
static int changedB = 0;



static void tick(void* param) {
    Sentry *sentry = (Sentry*) param;

    while(1) {
        size_t size = -1;
        void *dat = xRingbufferReceive(sentry->fifo, &size, portMAX_DELAY);
        if (dat == NULL || size <= 0) {
            continue;
        }
        Request * data = (Request *) dat;

        if(xSemaphoreTake(sentry->mutex, pdMS_TO_TICKS(10)) != pdTRUE){
            vRingbufferReturnItem(sentry->fifo, dat);
            continue;
        }


        if(sentry->pan.position != data->pan) {
            moveTo(&sentry->pan, data->pan);
            changedA++;
        }
        if(sentry->tilt.position != data->tilt) {
            moveTo(&sentry->tilt, data->tilt);
            changedB++;
        }

        vRingbufferReturnItem(sentry->fifo, dat);
        xSemaphoreGive(sentry->mutex);
//        vTaskDelay(1);
    }

}

static void tick2(void* param) {
    Sentry *sentry = (Sentry*) param;
    int changed = 0;
    while(1) {
//        ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(10));


        if(changedA + changedB > 0) {
            changedA = 0;
            changedB = 0;
            sendUpdateToClient(sentry);
            vTaskDelay(pdMS_TO_TICKS(250));
        }else{
            vTaskDelay(pdMS_TO_TICKS(1000));
        }



    }

}



void init(Sentry *sentry) {
    sentry->fd = -1;
    sentry->targetPan = 90;
    sentry->targetTilt = 90;
    sentry->mutex = xSemaphoreCreateMutex();
    sentry->fifo = xRingbufferCreate((sizeof(Request *)) * (64), RINGBUF_TYPE_NOSPLIT);
    if (sentry->fifo == NULL) {
        printf("Failed to create ring buffer\n");
    }
//    sentry->temp = NULL;
//    temperature_sensor_config_t temp_sensor = {
//            .range_min = 20,
//            .range_max = 50,
//    };
//    ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor, &sentry->temp));
    xTaskCreate(tick, "sentryTick", 2048, sentry, 6, &handle);
    xTaskCreatePinnedToCore(tick2, "sentryTick2", 4096, sentry, 4, NULL, 0);
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    sprintf(sentry->macAddress, "%x:%x:%x:%x:%x:%x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}
