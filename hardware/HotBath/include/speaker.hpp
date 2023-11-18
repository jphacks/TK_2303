#pragma once
#include "sound_data.hpp"
#include <Arduino.h>

namespace speaker
{
typedef enum {
    NoSound = 0,
    LongTime,
    Heat,
    Alert,
} Sound_Type;
void set_sound_and_restart(Sound_Type type);
void init_play_and_restart();
}  // namespace speaker