#include "sensor.hpp"
#include "SparkFun_SHTC3.h"
#include <Dps3xx.h>

#define I2C_PIN_SDA 42
#define I2C_PIN_SCL 2

namespace sensor
{
static constexpr uint8_t oversampling = 7;
static constexpr float alpha = 0.99f;

Dps3xx Dps3xxPressureSensor = Dps3xx();
SHTC3 shtc3;

static float temperature = 0;
static float pressure = 0;
static float humidity = 0;
static float humidity_filtered = 0;

void init()
{
    Wire.setPins(I2C_PIN_SDA, I2C_PIN_SCL);
    Dps3xxPressureSensor.begin(Wire);
    shtc3.begin();
}

void update()
{
    Dps3xxPressureSensor.measurePressureOnce(pressure, oversampling);
    Dps3xxPressureSensor.measureTempOnce(temperature, oversampling);
    shtc3.update();
    if (shtc3.lastStatus == SHTC3_Status_Nominal) {
        humidity = shtc3.toPercent();
    }

    humidity_filtered = alpha * humidity_filtered + (1 - alpha) * humidity;
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