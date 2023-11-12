#include "utils.hpp"

int64_t get_tick()
{
    return esp_timer_get_time() / 1000;
}