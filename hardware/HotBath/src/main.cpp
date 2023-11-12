#include "api.hpp"
#include "ble.hpp"
#include "config.hpp"
#include "led.hpp"
#include "onlineUpdate.hpp"
#include "sensor.hpp"
#include "utils.hpp"
#include "wifi.hpp"
#include <Arduino.h>

static void main_task(void* pvParameters);

void setup()
{
    Serial.begin(115200);
    update::rollback_check();
    config::init();
    sensor::init();
    led::init();

    api::init();
    xTaskCreatePinnedToCore(main_task, "main_task", 8196, NULL, 1, NULL, 1);
}

void main_task(void* pvParameters)
{
    update::check();
    ble::init();
    int64_t last_sensor_post = 0;
    int64_t last_firmware_check = 0;

    while (true) {
        wifi::update();
        sensor::update();
        if (get_tick() - last_sensor_post > 1000 * 60 * 10) {
            api::post_sensor_data(
                sensor::get_temperature(),
                sensor::get_pressure(),
                sensor::get_humidity());
        }
        if (get_tick() - last_firmware_check > 1000 * 60 * 24) {
            update::check();
        }
        vTaskDelay(1000);
    }
}

void loop()
{
    if (Serial.available() > 0) {
        char incoming = Serial.read();
        if (incoming == 'r') {
            ESP.restart();
        } else if (incoming == 'o') {
            ble::open_request();
        } else if (incoming == 'c') {
            ble::close_request();
        } else if (incoming == 'f') {
            config::factory_reset();
            ESP.restart();
        }
    }
    vTaskDelay(100);
}