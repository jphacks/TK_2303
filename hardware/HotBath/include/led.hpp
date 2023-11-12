#pragma once

namespace led
{
typedef enum {
    Normal = 0,
    Pairing,
} LED_Mode;

void init();
void set_mode(LED_Mode mode);
}  // namespace led