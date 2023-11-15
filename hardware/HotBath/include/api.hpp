#pragma once
#include "etl/string_view.h"

namespace api
{
typedef enum {
    BathIn = 0,
    BathOut,
    BathDanger
} Bath_Status;

void init();
bool post_sensor_data(float temperature, float pressure, float humidity);
bool post_bath_status(Bath_Status status);
}  // namespace api