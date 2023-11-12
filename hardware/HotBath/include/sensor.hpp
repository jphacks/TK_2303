#pragma once
#include <Arduino.h>

namespace sensor
{
void init();
void update();
float get_temperature();
float get_humidity();
float get_pressure();
}  // namespace sensor