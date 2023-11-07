#pragma once
#include "etl/string_view.h"

namespace api
{
void init();
bool post_sensor_data(int temperature, int humidity, int co2);
}  // namespace api