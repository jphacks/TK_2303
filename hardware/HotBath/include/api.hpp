#pragma once

namespace api
{
void init();
bool postSensorData(int temperature, int humidity, int co2);
}  // namespace api