#include "api.hpp"
#include "config.hpp"
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
    config::init();
    wifi::update();
    api::init();
    while (true) {
        wifi::update();
        api::post_sensor_data(1, 2, 3);
        vTaskDelay(60000);
    }
}

void loop()
{
    vTaskDelay(1000);
}