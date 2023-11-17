#include "sensor.hpp"
#include "SparkFun_SHTC3.h"
#include "api.hpp"
#include "mic.hpp"
#include "speaker.hpp"
#include "utils.hpp"
#include "wav.hpp"
#include <Arduino.h>
#include <Dps3xx.h>

#define I2C_PIN_SDA 42
#define I2C_PIN_SCL 2

namespace sensor
{
static constexpr uint8_t oversampling = 7;
static Dps3xx Dps3xxPressureSensor = Dps3xx();
static SHTC3 shtc3;

static const char TAG[] = "SENSOR";

static constexpr int64_t MAX_BATH_TIME = 10 * 1000;            // 30 min
static constexpr int64_t BATH_TIME_INTERVAL = 10 * 60 * 1000;  // 10 min

// temp
static constexpr float TEMP_ENTER_THRESHOLD = 1.0f;
static constexpr float TEMP_EXIT_THRESHOLD = 1.0f;
static constexpr float TEMP_EXIT_THRESHOLD_BEFORE_ENTER = TEMP_ENTER_THRESHOLD / 2.0f;
static constexpr float ALPHA_TEMP = 0.005f;
static constexpr float ALPHA_TEMP_LONG_TERM = 0.0001f;

static float temperature = 0;
static float temperature_filtered = 0;
static float temperature_filtered_long_term = 0;
static int16_t temperature_enter_possibility = 0;
static float max_bath_temperature = 0;
static float temperature_before_enter = 0;

// humid
static constexpr float HUMID_ENTER_THRESHOLD = 10.0f;
static constexpr float ALPHA_HUMID = 0.01f;

static float humidity = 0;
static float humidity_filtered = 0;
static int16_t humidity_enter_possibility = 0;

// pressure
static float pressure = 0;

// flags
static bool initialized = false;

static int64_t bath_in_time = 0;

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

static void sensor_task(void* pvParameters)
{
    {
        I2CLock lock;
        Wire.setPins(I2C_PIN_SDA, I2C_PIN_SCL);
        shtc3.begin();
        Dps3xxPressureSensor.begin(Wire);
    }

    while (true) {
        update();

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
                speaker::play(chime_sound, sizeof(chime_sound));
                speaker::play(heat_sound, sizeof(heat_sound));
                bath_in_time = get_tick();
                ESP_LOGI(TAG, "BathIn");
            }
            Serial.printf("N,%.2f,%.2f,%d,%.2f,%.2f,%d\n", temperature, temperature_filtered, temperature_enter_possibility, humidity, humidity_filtered, humidity_enter_possibility);
            break;
        case api::BathDanger:
        case api::BathIn:
            max_bath_temperature = max(max_bath_temperature, temperature);
            // Evaluate from the difference from the maximum temperature and the temperature before bathing
            if ((max_bath_temperature - temperature > TEMP_EXIT_THRESHOLD) || (temperature - temperature_before_enter < TEMP_EXIT_THRESHOLD_BEFORE_ENTER)) {
                api::set_bath_status(api::BathOut);
                ESP_LOGI(TAG, "BathOut");
            }
            if (get_tick() - bath_in_time > MAX_BATH_TIME) {
                // speaker::play(chime_sound, sizeof(chime_sound));
                // speaker::play(long_time_sound, sizeof(long_time_sound));
                WAVWriter wav_writer((uint8_t*)wav_buffer, sizeof(wav_buffer), 8000, 16);
                mic::record_to_wav(&wav_writer);

                bool safe = true;
                api::post_wav_data((uint8_t*)wav_buffer, sizeof(wav_buffer), safe);

                if (!safe) {
                    api::set_bath_status(api::BathDanger);
                    ESP_LOGI(TAG, "BathDanger");
                } else {
                    bath_in_time = get_tick() - MAX_BATH_TIME + BATH_TIME_INTERVAL;
                }
            }
            Serial.printf("I,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n", temperature, temperature_filtered, max_bath_temperature, humidity, humidity_filtered, temperature_before_enter);
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
    xMutex = xSemaphoreCreateRecursiveMutex();
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