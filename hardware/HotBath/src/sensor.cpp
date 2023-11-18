#include "sensor.hpp"
#include "SparkFun_SHTC3.h"
#include "api.hpp"
#include "mic.hpp"
#include "speaker.hpp"
#include "config.hpp"
#include "utils.hpp"
#include "wav.hpp"
#include <Arduino.h>
#include <Dps3xx.h>

#define I2C_PIN_SDA 42
#define I2C_PIN_SCL 2

#define DEMO

namespace sensor
{
static constexpr uint8_t oversampling = 7;
static Dps3xx Dps3xxPressureSensor = Dps3xx();
static SHTC3 shtc3;

static const char TAG[] = "SENSOR";

static constexpr int64_t MAX_BATH_TIME = 30 * 1000;  // 30 second

// temp
#ifdef DEMO
static constexpr float TEMP_ENTER_THRESHOLD = -5.0f;
static constexpr float TEMP_EXIT_THRESHOLD = 5.0f;
static constexpr float TEMP_EXIT_THRESHOLD_BEFORE_ENTER = 5.0f;
#else
static constexpr float TEMP_ENTER_THRESHOLD = 0.5f;
static constexpr float TEMP_EXIT_THRESHOLD = 1.0f;
static constexpr float TEMP_EXIT_THRESHOLD_BEFORE_ENTER = TEMP_ENTER_THRESHOLD / 2.0f;
#endif
static constexpr float ALPHA_TEMP = 0.005f;
static constexpr float ALPHA_TEMP_LONG_TERM = 0.0001f;

__NOINIT_ATTR float temperature;
__NOINIT_ATTR float temperature_filtered;
__NOINIT_ATTR float temperature_filtered_long_term;
__NOINIT_ATTR int16_t temperature_enter_possibility;
__NOINIT_ATTR float max_bath_temperature;
__NOINIT_ATTR float temperature_before_enter;

// humid
#ifdef DEMO
static constexpr float HUMID_ENTER_THRESHOLD = 3.0f;
#else
static constexpr float HUMID_ENTER_THRESHOLD = 10.0f;
#endif
static constexpr float ALPHA_HUMID = 0.01f;


__NOINIT_ATTR float humidity;
__NOINIT_ATTR float humidity_filtered;
__NOINIT_ATTR int16_t humidity_enter_possibility;

// pressure
static float pressure = 0;

// flags
__NOINIT_ATTR bool initialized;

__NOINIT_ATTR int64_t bath_in_time;
__NOINIT_ATTR int64_t last_vad_time;

static SemaphoreHandle_t xMutex = NULL;

I2CLock::I2CLock()
{
    xSemaphoreTakeRecursive(xMutex, portMAX_DELAY);
}
I2CLock::~I2CLock()
{
    xSemaphoreGiveRecursive(xMutex);
}

static void update()
{
    {
        I2CLock lock;
        shtc3.update();
        if (shtc3.lastStatus == SHTC3_Status_Nominal) {
            humidity = shtc3.toPercent();
        }
        Dps3xxPressureSensor.measureTempOnce(temperature, oversampling);
        Dps3xxPressureSensor.measurePressureOnce(pressure, oversampling);
    }

    // digital low pass filter
    if (initialized) {
        humidity_filtered = humidity * ALPHA_HUMID + (1 - ALPHA_HUMID) * humidity_filtered;
        temperature_filtered = temperature * ALPHA_TEMP + (1 - ALPHA_TEMP) * temperature_filtered;
        temperature_filtered_long_term = temperature * ALPHA_TEMP_LONG_TERM + (1 - ALPHA_TEMP_LONG_TERM) * temperature_filtered_long_term;
    } else {
        humidity_filtered = humidity;
        temperature_filtered = temperature;
        temperature_filtered_long_term = temperature;
        initialized = true;
    }
}

static void sensor_update_task(void* pvParameters)
{
    while (true) {
        update();
        vTaskDelay(1000);
    }
}

static void sensor_task(void* pvParameters)
{
    {
        I2CLock lock;
        Wire.begin(I2C_PIN_SDA, I2C_PIN_SCL);
        shtc3.begin();
        Dps3xxPressureSensor.begin(Wire);
    }
    update();
    xTaskCreatePinnedToCore(sensor_update_task, "sensor_update_task", 4096, NULL, 1, NULL, 1);

    while (true) {
        switch (api::get_bath_status()) {
        case api::BathNormal:
            // judge humidity
            if (humidity > 90) {
                humidity_enter_possibility = 100;  // True if humidity is above 90%.
            } else if (humidity - humidity_filtered > HUMID_ENTER_THRESHOLD) {
                humidity_enter_possibility = 100;
            } else {
                if (humidity_enter_possibility > 0) {
                    humidity_enter_possibility--;
                }
            }

            // judge temperature
            if (temperature - temperature_filtered > TEMP_ENTER_THRESHOLD) {
                temperature_enter_possibility = 100;
            } else {
                if (temperature_enter_possibility > 0) {
                    temperature_enter_possibility--;
                }
            }

            // judge integrated
            if (humidity_enter_possibility > 0 && temperature_enter_possibility > 0) {
                humidity_enter_possibility = 0;
                temperature_enter_possibility = 0;
                max_bath_temperature = -100;
                temperature_before_enter = temperature_filtered_long_term;
                api::set_bath_status(api::BathIn);
                api::post_alart("お風呂に入りました");
                ESP_LOGI(TAG, "BathIn");
                {
                    I2CLock i2c_lock;
                    speaker::play(chime_sound, sizeof(chime_sound));
                    speaker::play(bathin_sound, sizeof(bathin_sound));
                    speaker::play(heatshock_attention_sound, sizeof(heatshock_attention_sound));
                    bath_in_time = get_tick();
                    ESP.restart();
                }
            }
            Serial.printf("N,%.2f,%.2f,%d,%.2f,%.2f,%d\n", temperature, temperature_filtered, temperature_enter_possibility, humidity, humidity_filtered, humidity_enter_possibility);
            break;
        case api::BathIn:
            max_bath_temperature = max(max_bath_temperature, temperature);
// Evaluate from the difference from the maximum temperature and the temperature before bathing
#ifndef DEMO
            if ((max_bath_temperature - temperature > TEMP_EXIT_THRESHOLD) || (temperature - temperature_before_enter < TEMP_EXIT_THRESHOLD_BEFORE_ENTER)) {
                api::set_bath_status(api::BathOut);
                ESP_LOGI(TAG, "BathOut");
                {
                    I2CLock i2c_lock;
                    speaker::play(chime_sound, sizeof(chime_sound));
                    speaker::play(bathout_sound, sizeof(bathout_sound));
                    ESP.restart();
                }
                break;
            }
#endif
            if (get_tick() - bath_in_time > MAX_BATH_TIME) {
                api::set_bath_status(api::BathInLongTime);
                {
                    I2CLock i2c_lock;
                    speaker::play(chime_sound, sizeof(chime_sound));
                    speaker::play(long_time_sound, sizeof(long_time_sound));
                    ESP.restart();
                }
            }
            if (get_tick() - last_vad_time > 1000 * 60) {
                WAVWriter wav_writer((uint8_t*)wav_buffer, sizeof(wav_buffer), 8000, 16);
                mic::record_to_wav(&wav_writer);
                bool alive = true;
                api::post_wav_data_check_alive((uint8_t*)wav_buffer, sizeof(wav_buffer), alive);
                last_vad_time = get_tick();
            }
            Serial.printf("I,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n", temperature, temperature_filtered, max_bath_temperature, humidity, humidity_filtered, temperature_before_enter);
            break;
        case api::BathInLongTime: {
            WAVWriter wav_writer((uint8_t*)wav_buffer, sizeof(wav_buffer), 8000, 16);
            mic::record_to_wav(&wav_writer);
            bool safe = true;
            api::post_wav_data_check_safe((uint8_t*)wav_buffer, sizeof(wav_buffer), safe);
            if (!safe) {
                api::set_bath_status(api::BathDanger);
                api::post_alart("お風呂での異常を検知しました");
                ESP_LOGI(TAG, "BathDanger");
                {
                    I2CLock i2c_lock;
                    speaker::play(chime_sound, sizeof(chime_sound));
                    speaker::play(danger_sound, sizeof(danger_sound));
                    ESP.restart();
                }
                break;
            }
            bath_in_time = get_tick();
            api::set_bath_status(api::BathIn);
            break;
        }
        case api::BathDanger:
            ESP_LOGI(TAG, "in BathDanger");
            break;
        case api::BathOut:
            if (temperature - temperature_before_enter < TEMP_EXIT_THRESHOLD_BEFORE_ENTER) {
                api::set_bath_status(api::BathNormal);
                ESP_LOGI(TAG, "BathNormal");
            }
            Serial.printf("O,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n", temperature, temperature_filtered, temperature_filtered_long_term, humidity, humidity_filtered, temperature_before_enter);
            break;
        default:
            break;
        }
        vTaskDelay(1000);
    }
}

void init()
{
    if (config::is_first_boot()) {
        temperature = 0;
        temperature_filtered = 0;
        temperature_filtered_long_term = 0;
        temperature_enter_possibility = 0;
        max_bath_temperature = 0;
        temperature_before_enter = 0;

        humidity = 0;
        humidity_filtered = 0;
        humidity_enter_possibility = 0;

        initialized = false;
        bath_in_time = 0;
        last_vad_time = 0;
    }
    xMutex = xSemaphoreCreateRecursiveMutex();

    mic::init();
    xTaskCreatePinnedToCore(sensor_task, "sensor_task", 16384, NULL, 1, NULL, 1);
}

float get_temperature()
{
    return temperature;
}

float get_humidity()
{
    return humidity;
}

float get_pressure()
{
    return pressure / 100.0f;  // hPa
}
}  // namespace sensor