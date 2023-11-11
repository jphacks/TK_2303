#include "sensor.hpp"
#include "SparkFun_SHTC3.h"
#include <Dps3xx.h>

#define I2C_PIN_SDA 42
#define I2C_PIN_SCL 2

namespace sensor
{
static constexpr uint8_t oversampling = 7;

Dps3xx Dps3xxPressureSensor = Dps3xx();
SHTC3 shtc3;

void init()
{
    Wire.setPins(I2C_PIN_SDA, I2C_PIN_SCL);
    Dps3xxPressureSensor.begin(Wire);
    shtc3.begin();
}
float get_temperature()
{
    float temperature = 0;
    Dps3xxPressureSensor.measureTempOnce(temperature, oversampling);
    return temperature;
}
float get_humidity()
{
    shtc3.update();
    if (shtc3.lastStatus == SHTC3_Status_Nominal) {
        return shtc3.toPercent();
    }
    return 0;
}
float get_pressure()
{
    float pressure = 0;
    Dps3xxPressureSensor.measurePressureOnce(pressure, oversampling);
    return pressure;
}
}  // namespace sensor