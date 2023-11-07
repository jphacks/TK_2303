#include "api.hpp"
#include "ble.hpp"
#include "config.hpp"
#include "onlineUpdate.hpp"
#include "wifi.hpp"
#include <Arduino.h>

static void main_task(void* pvParameters);

void setup()
{
    Serial.begin(115200);
    xTaskCreatePinnedToCore(main_task, "main_task", 0x2000, NULL, 1, NULL, 1);
}


void main_task(void* pvParameters)
{
    update::rollback_check();
    config::init();
    wifi::update();
    api::init();
    update::begin();

    // ble::begin();

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