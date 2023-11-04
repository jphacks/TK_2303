#pragma once
#include "etl/string_view.h"

namespace ble
{
void begin();
void stop();
void send(etl::string_view data);
}  // namespace ble