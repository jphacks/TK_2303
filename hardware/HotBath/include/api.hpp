#pragma once
#include "etl/string_view.h"

namespace api
{
void init();
bool post_sensor_data(float temperature, float pressure, float humidity);
}  // namespace api