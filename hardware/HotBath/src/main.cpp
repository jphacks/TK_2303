#include "api.hpp"
#include "ble.hpp"
#include "config.hpp"
#include "led.hpp"
#include "online_update.hpp"
#include "secrets.hpp"
#include "sensor.hpp"
#include "speaker.hpp"
#include "utils.hpp"
#include "wifi.hpp"
#include "wav.hpp"
#include "mic.hpp"
#include <Arduino.h>

static void main_task(void* pvParameters);

void setup()
{
    Serial.begin(115200);
    update::rollback_check();
    config::init();

    // debug
    strncpy(config::data.ssid, MYSSID, sizeof(config::data.ssid));
    strncpy(config::data.pass, MYPASS, sizeof(config::data.pass));
    config::data.wifi_configured = true;
    strncpy(config::data.token, API_KEY, sizeof(config::data.token));
    config::data.token_configured = true;

    sensor::init();
    led::init();
    mic::init();
    api::init();
    xTaskCreatePinnedToCore(main_task, "main_task", 8192, NULL, 1, NULL, 1);
}

void main_task(void* pvParameters)
{
    update::check();
    ble::init();
    int64_t last_sensor_post = 0;
    int64_t last_firmware_check = 0;

    // speaker::play(heat_sound, sizeof(heat_sound));

    while (true) {
        wifi::update();

        static bool flag = false;
        if (!flag) {
            api::post_sensor_data(
                sensor::get_temperature(),
                sensor::get_pressure(),
                sensor::get_humidity());

            WAVWriter wav_writer((uint8_t*)wav_buffer, sizeof(wav_buffer), 8000, 16);
            mic::record_to_wav(&wav_writer);
            api::post_wav_data((uint8_t*)wav_buffer, sizeof(wav_buffer));
            flag = true;
        }

        if (get_tick() - last_sensor_post > 1000 * 60 * 10) {
            api::post_sensor_data(
                sensor::get_temperature(),
                sensor::get_pressure(),
                sensor::get_humidity());
            last_sensor_post = get_tick();
        }
        if (get_tick() - last_firmware_check > 1000 * 60 * 24) {
            update::check();
            last_firmware_check = get_tick();
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