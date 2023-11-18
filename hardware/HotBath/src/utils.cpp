#include "utils.hpp"
#include "config.hpp"
#include <Arduino.h>

int64_t get_tick()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL));
}