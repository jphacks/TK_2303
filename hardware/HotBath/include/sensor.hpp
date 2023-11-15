#pragma once
#include <Arduino.h>

namespace sensor
{
class I2CLock
{
public:
    I2CLock();
    ~I2CLock();
};

void init();
void update();
float get_temperature();
float get_humidity();
float get_pressure();
}  // namespace sensor