#include "api.hpp"
#include "wifi.hpp"
#include <Arduino.h>

static void mainTask(void* pvParameters);

void setup()
{
    Serial.begin(115200);
    xTaskCreatePinnedToCore(mainTask, "mainTask", 0x2000, NULL, 1, NULL, 1);
}

void mainTask(void* pvParameters)
{
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