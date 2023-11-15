#pragma once
#include <Arduino.h>

#define I2S_PIN_WS 25
#define I2S_PIN_DIN 26

#define I2S_SAMPLE_RATE 32000
#define I2S_BUFFER_COUNT 4
#define I2S_BUFFER_SIZE 64

namespace mic
{
void micTask(void* param)
{
    while (true) {
        vTaskDelay(10);
    }
}

void setup()
{
    xTaskCreatePinnedToCore(micTask, "MicTask", 8196, NULL, 2, NULL, 0);
}
}  // namespace mic
