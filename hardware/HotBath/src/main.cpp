#include "HttpsOTAUpdate.h"
#include "api.hpp"
#include "ble.hpp"
#include "config.hpp"
#include "wifi.hpp"
#include <Arduino.h>

static void main_task(void* pvParameters);

void setup()
{
    Serial.begin(115200);
    xTaskCreatePinnedToCore(main_task, "main_task", 0x2000, NULL, 1, NULL, 1);
}


static HttpsOTAStatus_t otastatus;

void HttpEvent(HttpEvent_t* event)
{
    switch (event->event_id) {
    case HTTP_EVENT_ERROR:
        Serial.println("Http Event Error");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        Serial.println("Http Event On Connected");
        break;
    case HTTP_EVENT_HEADER_SENT:
        Serial.println("Http Event Header Sent");
        break;
    case HTTP_EVENT_ON_HEADER:
        Serial.printf("Http Event On Header, key=%s, value=%s\n", event->header_key, event->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        break;
    case HTTP_EVENT_ON_FINISH:
        Serial.println("Http Event On Finish");
        break;
    case HTTP_EVENT_DISCONNECTED:
        Serial.println("Http Event Disconnected");
        break;
    }
}

static const char* url = "https://example.com/firmware.bin";

void main_task(void* pvParameters)
{
    config::init();
    ble::begin();
    wifi::update();
    api::init();

    HttpsOTA.onHttpEvent(HttpEvent);
    Serial.println("Starting OTA");
    HttpsOTA.begin(url, server_certificate);

    while (true) {
        wifi::update();
        api::post_sensor_data(1, 2, 3);
        vTaskDelay(60000);
    }
}


void loop()
{
    if (Serial.available() > 0) {
        char incoming = Serial.read();
        if (incoming == 'r') {
            ESP.restart();
        } else if (incoming == 'b') {
            ble::begin();
        } else if (incoming == 'e') {
            ble::stop();
        }
    }
    vTaskDelay(100);
}