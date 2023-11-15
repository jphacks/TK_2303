#pragma once
#include "sound_data.hpp"
#include <Arduino.h>

namespace speaker
{
void play(const uint16_t* audio_data, size_t remain);
}  // namespace speaker