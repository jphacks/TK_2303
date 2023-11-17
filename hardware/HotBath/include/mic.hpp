#pragma once
#include <Arduino.h>
#include <wav.hpp>

namespace mic
{
class MicLock
{
public:
    MicLock();
    ~MicLock();
};

void init();
void record_to_wav(WAVWriter* wav_writer);
}  // namespace mic
