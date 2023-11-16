#pragma once
#include <Arduino.h>
#include <wav.hpp>

namespace mic
{
void init();
void record_to_wav(WAVWriter* wav_writer);
}  // namespace mic
