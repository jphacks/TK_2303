#include "sensor.hpp"
#include "SparkFun_SHTC3.h"
#include "api.hpp"
#include "speaker.hpp"
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

// temp
static constexpr float TEMP_ENTER_THRESHOLD = 2.0f;
static constexpr float TEMP_EXIT_THRESHOLD = 5.0f;
static constexpr float TEMP_EXIT_THRESHOLD_BEFORE_ENTER = TEMP_ENTER_THRESHOLD / 2.0f;
static constexpr float ALPHA_TEMP = 0.01f;
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
static bool is_entered = false;
static bool initialized = false;

static SemaphoreHandle_t xMutex = NULL;

I2CLock::I2CLock()
{
    xSemaphoreTakeRecursive(xMutex, portMAX_DELAY);
}
I2CLock::~I2CLock()
{
    xSemaphoreGiveRecursive(xMutex);
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
        // update sensor data
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

        if (is_entered) {  // bath entered
            max_bath_temperature = max(max_bath_temperature, temperature);
            // Evaluate from the difference from the maximum temperature and the temperature before bathing
            if ((max_bath_temperature - temperature > TEMP_EXIT_THRESHOLD) || (temperature - temperature_before_enter < TEMP_EXIT_THRESHOLD_BEFORE_ENTER)) {
                is_entered = false;
                api::post_bath_status(api::BathOut);
                ESP_LOGI(TAG, "bath exited");
            }
            Serial.printf("E,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n", temperature, temperature_filtered, max_bath_temperature, humidity, humidity_filtered, temperature_before_enter);
        } else {  // bath not entered
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
                is_entered = true;
                humidity_enter_possibility = 0;
                temperature_enter_possibility = 0;
                max_bath_temperature = -100;
                temperature_before_enter = temperature_filtered_long_term;
                api::post_bath_status(api::BathIn);
                speaker::play(heat_sound, sizeof(heat_sound));
                ESP_LOGI(TAG, "bath entered");
            }
            Serial.printf("N,%.2f,%.2f,%d,%.2f,%.2f,%d\n", temperature, temperature_filtered, temperature_enter_possibility, humidity, humidity_filtered, humidity_enter_possibility);
        }
        vTaskDelay(1000);
    }
}

void init()
{
    xMutex = xSemaphoreCreateRecursiveMutex();
    xTaskCreatePinnedToCore(sensor_task, "sensor_task", 8192, NULL, 1, NULL, 1);
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